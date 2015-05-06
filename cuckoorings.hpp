/** @class CuckooRings
 * @brief This data structures combines two consistent hashing rings into
 * a single data structure which uses cuckoo-like rehashing to ensure both
 * that keys don't often need to get rehashed.
 *
 * @author ankitvgupta
 * @author jonahkall
 */
#include <iostream>
#include <string>
#include <set>
#include <functional>
#include <map>
#include <vector>

#include "ringhash.hpp"
#include "farmhash.hpp"

#define STOP_ITERS 8

class CuckooRings {
private:
  typedef long long server_id;
  typedef std::map<long long, std::vector<int> > MapType;
  typedef MapType::iterator MapIterator;
  typedef std::vector<int>::iterator VectorIterator;

  /**
   * Key space size. Indicates what the maximum key to be hashed to is
   */
  long long kss_;

  /*
   * Tracks the number of servers
   */
  int num_servers_;

  /**
   * Stores pointers to the left and right rings
   */
  RingHash* left_ring_;
  RingHash* right_ring_;

  unsigned insert_counter;

public:

  /*
   * @brief THis is the hash function to be used by left HashRing
   * @param a the number that being hashed
   * @param kss_ the key space size
   * @returns a hash between 0 and kss_
   * Source: Thomas Wang, http://burtleburtle.net/bob/hash/integer.html
   */
  static
  long long hash_left(long long a, long long kss_) {
    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    a = (a+0x165667b1) + (a<<5);
    a = (a+0xd3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3);
    a = (a^0xb55a4f09) ^ (a>>16);
    return abs(((a % kss_) + kss_) % kss_);
  }

  /*
   * @brief THis is the hash function to be used by right HashRing
   * @param a the number that being hashed
   * @param kss_ the key space size
   * @returns a hash between 0 and kss_
   * Source: Thomas Wang via Geoffrey Irving, https://naml.us/blog/tag/thomas-wang
   */
  static
  long long hash_right(long long key, long long kss_) {
  key = (~key) + (key << 21); // key = (key << 21) - key - 1;
  key = key ^ (key >> 24);
  key = (key + (key << 3)) + (key << 8); // key * 265
  key = key ^ (key >> 14);
  key = (key + (key << 2)) + (key << 4); // key * 21
  key = key ^ (key >> 28);
  key = key + (key << 31);
  
  return abs(((key % kss_) + kss_) % kss_);
}


  /**
   * Constructor for CuckooRings
   * @param key_space_size indicates that the max key that can be hashed to
   * @param init_servers indicates the number of servers in EACH ring
   */
  CuckooRings(long long key_space_size, int init_servers) :
      kss_(key_space_size) {
    // Set up keyspace now
    left_ring_ = new RingHash(key_space_size, init_servers, hash_left);
    right_ring_ = new RingHash(key_space_size, init_servers, hash_right);
    num_servers_ = 2 * init_servers;
  }

  /**
   * #param the key that is being inserted
   * @brief Inserts a key into the HashRing
   */
  void insert (int key) {
    insert_counter = 0;
    server_id ret;
    if (left_ring_->num_keys() > right_ring_->num_keys()) {
      ret = right_ring_->insert(key);
      if (ret != -1) {
        send_server_rtol(ret);
      }
    }
    else {
      ret = left_ring_->insert(key);
      if (ret != -1) {
        send_server_ltor(ret);
      }
    }
  }

  /**
   * @brief Sends all the contents of a server from the left ring to the right ring
   * @param s the server_id of the server in the Left Ring that is being cuckooed over
   */
  void send_server_ltor(server_id s) {
    ++insert_counter;
    if (insert_counter > STOP_ITERS) {
      return;
    }
    server_id ret;
    vector<server_id> to_send;
    vector<int>& lserver = left_ring_->get_keys(s);
    for (const auto& i : lserver) {
      ret = right_ring_->insert(i);
      if (ret != -1) {
        to_send.push_back(ret);
      }
    }
    left_ring_->clear_server(s);
    for (const auto& server : to_send) {
      send_server_rtol(server);
    }
  }

  /**
   * @brief Sends all the contents of a server from the right ring to the left ring
   * @param s the server_id of the server in the right ring that is being cuckooed over
   */
  void send_server_rtol(server_id s) {
    ++insert_counter;
    if (insert_counter > STOP_ITERS) {
      return;
    }
    server_id ret;
    vector<server_id> to_send;
    vector<int>& rserver = right_ring_->get_keys(s);
    for (const auto& i : rserver) {
      ret = left_ring_->insert(i);
      if (ret != -1) {
        to_send.push_back(ret);
      }
    }
    right_ring_->clear_server(s);
    for (const auto& server : to_send) {
      send_server_ltor(server);
    }
  }

  /**
   * #param the key that is being removed
   * @brief removes a key into the HashRing
   * @brief Can be implemented by checking each ring and removing,
   *    but we left it out since we are not using it.
   */
  void remove(int key) {
    (void)key;
  }

  /**
   * #param the key that is being looked up
   * @brief Finds the server associated with a key
   * @returns server_id of the associated server
   * @brief Not implemented because not needed for us.
   */
  server_id lookup (int key) {
    (void)key;
    return 0;
  }

  /**
   * #param location where server will be put
   * @brief adds a server to the RingHash
   *    not implemented because not needed for us
   * @returns void
   */
  void add_server(int server_loc) {
    (void)server_loc;
  }



  /**
   * @brief Removes a random server from the specified side
   * @param side The side to remove the server from
   */
  // removes from left if side = 0, from right if side = 1
  void remove_random_server(server_id s, int side) {
    (void) s;
    if (side == 0){
      left_ring_->remove_random_server();
    }
    else{
      right_ring_->remove_random_server();
    }
    return;
  }

  /**
   * @brief Prints the loads on each server, 
   *    separated by a comma, followed by the total load
   */
  void print_loads(void) {
    cout << "Left ring loads are: " << "\n";
    left_ring_->print_loads();
    cout << "Right ring loads are: " << "\n";
    right_ring_->print_loads();
  }

  /**
   * @brief evaluates the cost of a CuckooRing by determing the cost of each ring
   * @returns the cost as a long long
   */
  long long cost_of_structure(void){
    return (left_ring_->cost_of_structure() + right_ring_->cost_of_structure());
  }

  /**
   * @brief finds the server with the highest load
   * @returns the load of that server
   */
  long long get_max_load(void) {
    return max(right_ring_->get_max_load(), left_ring_->get_max_load());
  }

  /**
   * @brief finds the server with the least load
   * @returns the load of that server
   */
  long long get_min_load(void){
    return min(left_ring_->get_min_load(), left_ring_->get_min_load());
  }

  /**
   * @brief determines the number of servers in the CuckooRing
   * @returns that number as a long long
   */
  long long getNumServers(void){
    return (left_ring_->getNumServers() + right_ring_->getNumServers());
  }

  /**
   * @brief finds the number of keys inserted in the structure
   * @returns that number as a long long
   */
  long long getNumKeys(void){
    return (left_ring_->getNumKeys() + right_ring_->getNumKeys());
  }

  /**
   * @brief adds a server to a random location in the specified side
   */
  void add_random_server(int side){
    side = side % 2;
    if (side == 0) {
      left_ring_->add_random_server();
    }
    else {
      right_ring_->add_random_server();
    }
    return;

  }

};
