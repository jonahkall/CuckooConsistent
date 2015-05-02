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

#define STOP_ITERS 8

class CuckooRings {
private:
  typedef long long server_id;
  typedef std::map<long long, std::vector<int> > MapType;
  typedef MapType::iterator MapIterator;
  typedef std::vector<int>::iterator VectorIterator;

  // This will be the key space size for both the left and the right rings.
  long long kss_;
  int num_servers_;
  RingHash* left_ring_;
  RingHash* right_ring_;
  unsigned insert_counter;
public:
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

  //TODO: this better be a more different hash function lol
  static
  long long hash_right(long long a, long long kss_) {
    a = (a+0x7ed55d15) + (a<<12);
    a = (a^0xc741c20c) ^ (a>>19);
    a = (a+0x125667b1) + (a<<5);
    a = (a+0xf3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3);
    a = (a^0xb55b4f09) ^ (a>>16);
    return abs(((a % kss_) + kss_) % kss_);
  }
    // We add init_servers servers to both rings
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
    // if (left_ring_->num_keys() > right_ring_->num_keys()) {
    //   right_ring_->insert(key);
    //   return;
    // }
    //left_ring_->insert(key);
    insert_counter = 0;
    server_id ret;
    if (left_ring_->num_keys() > right_ring_->num_keys()) {
      ret = right_ring_->insert(key);
      if (ret != -1) {
        //cout << "sending over\n";
        send_server_rtol(ret);
      }
    }
    else {
      ret = left_ring_->insert(key);
      if (ret != -1) {
        //cout << "sending over\n";
        send_server_ltor(ret);
      }
    }
  }

  // Sends a particular server id's keys over to the other server.
  void send_server_ltor(server_id s) {
    ++insert_counter;
    if (insert_counter > STOP_ITERS) {
      cout << "gave up\n";
      return;
    }
    //cout << "send l to r\n";
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

  void send_server_rtol(server_id s) {
    ++insert_counter;
    if (insert_counter > STOP_ITERS) {
      cout << "Gave up\n";
      return;
    }
    //cout << "send r to l\n";
    server_id ret;
    vector<server_id> to_send;
    vector<int>& rserver = right_ring_->get_keys(s);
    for (const auto& i : rserver) {
      ret = left_ring_->insert(i);
      if (ret != -1) {
        to_send.push_back(ret);
        //send_server_ltor(ret);
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
   */
  void remove(int key) {
    (void)key;
  }

  /**
   * #param the key that is being looked up
   * @brief Finds the server associated with a key
   * @returns server_id of the associated server
   */
  server_id lookup (int key) {
    (void)key;
    return 0;
  }

  /**
   * #param location where server will be put
   * @brief adds a server to the RingHash
   * @returns void
   */
   // NOTE: we should add these at some point but we cant punt them for
   // now because they aren't critical for our testing.
   // return out the server id where the addition was done
  void add_server(int server_loc) {
    (void)server_loc;
  }

  void remove_server(server_id s) {
    (void)s;
  }

  /**
   * @brief Prints the loads on each server, separated by a comma, followed by the total load
   */
  void print_loads(void) {
    cout << "Left ring loads are: " << "\n";
    left_ring_->print_loads();
    cout << "Right ring loads are: " << "\n";
    right_ring_->print_loads();
  }

  /**
   * @brief finds the server with the highest load
   * @returns the server id of that server
   */
  long long get_max_load(void){
    // just take the max of the two

    // NOTE: this is currently wrong!!!!!!!!!!!!
    // just for testing
    return max(right_ring_->get_max_load(), left_ring_->get_max_load());
  }

  long long get_min_load(void){
    return min(left_ring_->get_min_load(), left_ring_->get_min_load());
  }
};
