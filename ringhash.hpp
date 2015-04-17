/** @class RingHash
 * @brief A fast consistent hashing ring.
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

using namespace std;

class RingHash {
private: 
  typedef int server_id;
  typedef std::map<unsigned int, std::vector<int> > MapType;
  typedef MapType::iterator MapIterator;
  // This is a red-black tree. O(log n) insertions,
  // deletions, and lookups.
  // server to its load
  MapType cache_indices_;
  long long kss_;
  int num_servers_;
  unsigned hash(long long a) const {
    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    a = (a+0x165667b1) + (a<<5);
    a = (a+0xd3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3);
    a = (a^0xb55a4f09) ^ (a>>16);
    return abs(((a % kss_) + kss_) % kss_);
  }
public:
  RingHash(long long key_space_size, int init_servers) :
      kss_(key_space_size), num_servers_(init_servers) {
    // Set up keyspace now
    for (int i = 0; i < init_servers; ++i) {
      cache_indices_.insert(std::make_pair(i* (key_space_size/init_servers), std::vector<int>()));
    }
  }

  /**
   * #param the key that is being inserted
   * @brief Inserts a key into the HashRing
   */
  void insert (int key) {
    server_id tmp = lookup(key);
    cache_indices_[tmp].push_back(key); 
  }

  /**
   * #param the key that is being removed
   * @brief removes a key into the HashRing
   */
  void remove(int key) {


    server_id tmp = lookup(key);
    std::vector<int>::iterator it;

    std::vector<int> vals = cache_indices_[tmp];
    //cout << "beginning" << cache_indices_[tmp].size() << endl;

    for (it=cache_indices_[tmp].begin(); it != cache_indices_[tmp].end(); ++it){
      if (*it == key) {
        //cout << "Found it" << endl;
        cache_indices_[tmp].erase(it);
        break;
      }
    }

    //cout << "end" << cache_indices_[tmp].size() << endl;




    //cache_indices_[tmp].erase(key); 
  }

  /**
   * #param the key that is being looked up
   * @brief Finds the server associated with a key
   * @returns server_id of the associated server
   */
  server_id lookup (int key) {
    unsigned int tmp = hash(key);
    MapIterator m = cache_indices_.lower_bound(tmp);
    //cout << m->first << endl;
    return m->first;
  }


  /**
   * #param location where server will be put
   * @brief adds a server to the RingHash
   * @returns void
   */
  void add_server(int server_loc) {
    ++num_servers_;

    std::vector<int>::iterator it;

    server_id server_to_bump = cache_indices_.lower_bound(server_loc)->first; // note that this works since we are putting the new server at the location of its hash

    // make a copy of the set
    std::vector<int> keys_to_bump(cache_indices_[server_to_bump]);

    // clear the original one
    cache_indices_[server_to_bump].clear();

    // add the new server
    cache_indices_.insert(std::make_pair(server_loc, std::vector<int>()));

    // rehash the keys
    for (it=keys_to_bump.begin(); it != keys_to_bump.end(); ++it){
      insert(*it);
    }


  }
  /**
   * #param the server_id of the server being removed
   * @brief removes a server to the RingHash
   * @returns void
   */
  void remove_server(server_id s) {

    std::vector<int>::iterator it;

    // make a copy of the keys in the server that will be removed
    std::vector<int> keys_to_bump(cache_indices_[s]);

    // remove the given server
    cache_indices_.erase(s);

    // rehash the keys
    for (it=keys_to_bump.begin(); it != keys_to_bump.end(); ++it){
      insert(*it);
    }

    --num_servers_;

  }

  /**
   * @brief Prints the loads on each server, separated by a comma, followed by the total load
   */
  void print_loads(void) {
    int total = 0;
    for (const auto&x : cache_indices_) {
      //cout << x.first << " ";
      cout << x.second.size() << " ";
      total += x.second.size();
    }
    cout << "Total load is " << total << endl; 
  }

};