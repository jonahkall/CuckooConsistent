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

long long hashstd(long long a, long long kss_) {
  a = (a+0x7ed55d16) + (a<<12);
  a = (a^0xc761c23c) ^ (a>>19);
  a = (a+0x165667b1) + (a<<5);
  a = (a+0xd3a2646c) ^ (a<<9);
  a = (a+0xfd7046c5) + (a<<3);
  a = (a^0xb55a4f09) ^ (a>>16);
  return abs(((a % kss_) + kss_) % kss_);
}

class RingHash {
private:
  typedef long long server_id;
  typedef std::map<long long, std::vector<int> > MapType;
  typedef MapType::iterator MapIterator;
  typedef std::vector<int>::iterator VectorIterator;
  // This is a red-black tree. O(log n) insertions,
  // deletions, and lookups.
  // server to its load
  MapType cache_indices_;
  long long kss_;
  int num_servers_;
  int num_keys_;
  std::function<long long(long long, long long)> hash;
public:
  friend class CuckooRings;
  RingHash(long long key_space_size, int init_servers) :
      kss_(key_space_size), num_servers_(init_servers) {
    num_keys_ = 0;
    hash = hashstd;
    // Set up keyspace now
    for (int i = 0; i < init_servers; ++i) {
      cache_indices_.insert(std::make_pair(i* (key_space_size/init_servers), std::vector<int>()));
    }
  }

  RingHash(long long key_space_size, int init_servers,
      std::function<long long(long long, long long)> hashfn) :
      kss_(key_space_size), num_servers_(init_servers) {
    num_keys_ = 0;
    // Set up keyspace now
    for (int i = 0; i < init_servers; ++i) {
      cache_indices_.insert(std::make_pair(i* (key_space_size/init_servers), std::vector<int>()));
    }
    hash = hashfn;
  }

  ~RingHash() {
    cout << "called ringhash destructor\n";
  }

  int size (void) {
    return cache_indices_.size();
  }

  int num_keys(void) {
    return num_keys_;
  }

  /**
   * #param the key that is being inserted
   * @brief Inserts a key into the HashRing
   */
  void insert (int key) {
    ++num_keys_;
    server_id tmp = lookup(key);
    cache_indices_[tmp].push_back(key); 
  }

  /**
   * #param the key that is being removed
   * @brief removes a key into the HashRing
   */
  void remove(long long key) {

    server_id tmp = lookup(key);
    VectorIterator it;

    std::vector<int> vals = cache_indices_[tmp];

    // iterate over the vector and remove the given key
    for (it=cache_indices_[tmp].begin(); it != cache_indices_[tmp].end(); ++it){
      if (*it == key) {
        cache_indices_[tmp].erase(it);
        break;
      }
    }
  }

  /**
   * #param the key that is being looked up
   * @brief Finds the server associated with a key
   * @returns server_id of the associated server
   */
  server_id lookup (long long key) {
    long long tmp = hash(key, kss_);

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
    VectorIterator it;
    server_id server_to_bump = cache_indices_.lower_bound(server_loc)->first;
    // note that this works since we are putting the new server at the location of its hash

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

    VectorIterator it;

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

  // For usage in CuckooRings
  void remove_server_no_rehash(server_id s) {

    VectorIterator it;

    // make a copy of the keys in the server that will be removed
    std::vector<int> keys_to_bump(cache_indices_[s]);

    // remove the given server
    cache_indices_.erase(s);

    --num_servers_;
  }

  /**
   * @brief Prints the loads on each server, separated by a comma, followed by the total load
   */
  void print_loads(void) {
    int total = 0;
    for (const auto&x : cache_indices_) {
      cout << "Load on: " << x.first << " is: " << x.second.size() << "\n";
      total += x.second.size();
    }
    cout << "Max load server is: " << get_max_load_server() << "\n";
    cout << "Total load is " << total << endl; 
  }

  /**
   * @brief finds the server with the highest load
   * @returns the server id of that server
   */
  server_id get_max_load_server(void){
    server_id highest_server = 0;
    int sz = 0;
    for (const auto&x : cache_indices_) {
      if (x.second.size() > sz) {
        highest_server = x.first;
        sz = x.second.size();
      }
    }
    return highest_server;
  }

  vector<int>& get_keys(server_id s) {
    return cache_indices_[s];
  }

};