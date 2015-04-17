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
    (void)key;
  }

  void remove(int key) {
    (void)key;
  }

  server_id lookup (int key) {
    int tmp = hash(key);
    MapIterator m = cache_indices_.lower_bound(tmp);
    return m->first;
  }

  void add_server(server_id s) {
    ++num_servers_;
    (void)s;
  }

  void remove_server(server_id s) {
    (void)s;
  }

  void print_loads(void) {
    for (const auto&x : cache_indices_) {
      cout << x.second.size();
    }
  }

};