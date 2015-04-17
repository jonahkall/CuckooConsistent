/** @class RingHash
 * @brief A fast consistent hashing ring.
 *
 * 
 */
#include <iostream>
#include <string>
#include <set>
#include <functional>
#include <map>

using namespace std;

class RingHash {
private: 
  typedef int server_id;
  typedef std::map<int, std::set<int> > MapType;
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
      cache_indices_.insert(std::make_pair(i* (key_space_size/init_servers), std::set<int>()));
    }
  }

  void insert (int key) {
    server_id tmp = lookup(key);
    cache_indices_[tmp].insert(key); 
  }

  void remove(int key) {
    server_id tmp = lookup(key);
    cache_indices_[tmp].erase(key); 
  }

  server_id lookup (int key) {
    int tmp = hash(key);
    MapIterator m = cache_indices_.lower_bound(tmp);
    return m->first;
  }

  void add_server(server_id s) {
    ++num_servers_;

    std::set<int>::iterator it;

    int server_loc = hash(s); // this can perhaps be user specified or determined some other way

    server_id server_to_bump = lookup(s); // note that this works since we are putting the new server at the location of its hash

    // make a copy of the set
    std::set<int> keys_to_bump(cache_indices_[server_to_bump]);

    // clear the original one
    cache_indices_[server_to_bump].clear();

    // add the new server
    cache_indices_.insert(std::make_pair(server_loc, std::set<int>()));

    // rehash the keys
    for (it=keys_to_bump.begin(); it != keys_to_bump.end(); ++it){
      insert(*it);
    }


  }

  void remove_server(server_id s) {


    std::set<int>::iterator it;
    std::set<int> keys_to_bump(cache_indices_[s]);

    cache_indices_.erase(s);

    // rehash the keys
    for (it=keys_to_bump.begin(); it != keys_to_bump.end(); ++it){
      insert(*it);
    }

    --num_servers_;
  }

  void print_loads(void) {
    for (const auto&x : cache_indices_) {
      cout << x.second.size();
    }
  }

};