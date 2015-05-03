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
#include <math.h>
#include <vector>

#define SERVER_THRESHOLD 2

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
  typedef std::map<unsigned long long, std::vector<int> > MapType;
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
      cache_indices_.insert(std::make_pair( (long long) (i* (((double)key_space_size)/init_servers)),
          std::vector<int>()));
    }
  }

  RingHash(long long key_space_size, long long init_servers,
      std::function<long long(long long, long long)> hashfn) :
      kss_(key_space_size), num_servers_(init_servers) {
    num_keys_ = 0;
    // Set up keyspace now
    for (int i = 0; i < init_servers; ++i) {
      cache_indices_.insert(std::make_pair( (long long) (i* (((double)key_space_size)/init_servers)),
          std::vector<int>()));
    }
    auto x = cache_indices_.begin();
    long long val = x->first;
    long long first_diff;
    int count = 0;
    while (x != cache_indices_.end())
    {
      ++count;
        ++x;
        if (count == 1)
        {
          first_diff = x->first - val;
          cout << first_diff << endl;
        }
        if (x == cache_indices_.end()){
          break;
        }
        //cout << "added " << x->first - val << endl;
        if (x->first - val != first_diff) {
          //cout << "we fucked up\n";
        }
        val = x->first;
    }
    cout << (1L << 32) - cache_indices_.rbegin()->first << endl;
    hash = hashfn;
  }

  ~RingHash() {}

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
   // return -1 if insertion is fine, serverid otherwise
  server_id insert (int key) {
    ++num_keys_;
    server_id tmp = lookup(key);
    cache_indices_[tmp].push_back(key); 
    if (cache_indices_[tmp].size() > SERVER_THRESHOLD) {
      return tmp;
    }
    else {
      return -1;
    }
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
        --num_keys_;
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
    
    //cout << tmp << endl;

    //assert(tmp != 0);
    MapIterator m = cache_indices_.lower_bound(tmp);
    if (m == cache_indices_.begin()){
      cout << "This is bad hash" << endl;
    }
    //assert(m != cache_indices_.begin());
    if (m == cache_indices_.end()){
      //cout << "bad" << endl;
      m = cache_indices_.begin();

    }
    //cout << m->first << endl;
    return m->first;
  }


  /**
   * #param location where server will be put
   * @brief adds a server to the RingHash
   * @returns void
   */
  void add_server(int server_loc) {
    
    VectorIterator it;
    //server_id server_to_bump = cache_indices_.lower_bound(server_loc)->first;
    MapIterator m = cache_indices_.lower_bound(server_loc);
    if (m == cache_indices_.end()){
      m = cache_indices_.begin();
    }
    server_id server_to_bump = m->first;
    // note that this works since we are putting the new server at the location of its hash

    // make a copy of the set
    std::vector<int> keys_to_bump(cache_indices_[server_to_bump]);

    // clear the original one
    cache_indices_[server_to_bump].clear();

    // add the new server
    cache_indices_.insert(std::make_pair(server_loc, std::vector<int>()));
    ++num_servers_;

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
    
  // TODO: test this
  void clear_server(server_id s) {
    cache_indices_[s].clear();
    return;
  }

  // For usage in CuckooRings
  // @Deprecated
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

  long long get_max_load(void) {
    int sz = 0;
    for (const auto&x : cache_indices_) {
      if (x.second.size() > sz) {
        sz = x.second.size();
      }
    }
    return sz;
  }

  long long get_min_load(void) {
    int sz = 99999999;
    for (const auto&x : cache_indices_) {
      if (x.second.size() < sz) {
        sz = x.second.size();
      }
    }
    return sz;
  }  

  float get_avg_load(void){
    long long tot = 0;
    for (const auto&x : cache_indices_) {
      tot += x.second.size();
    }
    //cout << tot << endl;
    //cout << size() << endl;
    float avg = ((float) tot)/((float) size());
    //cout << avg << endl;
    return avg;
  }

  long long get_variance_load(void){
    float tot = 0;
    float avg = get_avg_load();
    float sz = (float) size();
    for (const auto&x : cache_indices_) {
      tot += pow((((float) x.second.size()) - avg), 2.0);
    }
    return tot/sz;

  }

  // Remove a random server from the Ring
  //here
  void remove_random_server(void){
    MapIterator m = cache_indices_.lower_bound(rand() % kss_);
    if (m == cache_indices_.end()){
      m = cache_indices_.begin();
    }
    server_id s =  m->first;
    remove_server(s);

    //cout << "random server id to delete: " << s << endl;
  }

  // Determines the cost of a particular server based on how many elements are in it
  long long costfunction(int sz) {
    return pow(sz, 2.0);
  }
  // Determine the cost of the ring (for example, having a squared penalty based on load)
  long long cost_of_structure(void){
    long long cost = 0;
    for (const auto&x : cache_indices_) {
      cost += costfunction(x.second.size());
    }
    return cost;
  }

  long long getNumKeys(void){
    long long tot = 0;
    for (const auto&x : cache_indices_) {
      tot += x.second.size();
    }
    return tot;
  }

  long long getNumServers(void){
    long long tot = 0;
    for (const auto&x : cache_indices_) {
      tot += 1;
    }
    return tot;
  }

  void add_random_server(void){
    int randnum = 0;

    // make sure that the server is not already in there.
    do {
      randnum = rand();
    } while (cache_indices_.find(randnum) != cache_indices_.end());

    add_server(randnum);
  }

  vector<int>& get_keys(server_id s) {
    return cache_indices_[s];
  }

};
