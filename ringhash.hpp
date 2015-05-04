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
#include <climits>

// 2, 5, 10
#define SERVER_THRESHOLD 10

using namespace std;

/**
 * @brief A standard hash function used in the absense of one passed in
   * @param a the number that being hashed
   * @param kss_ the key space size
   * @returns a hash between 0 and kss_
 */
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

  /**
   * Create a std::map that forms consistent hashing rings. The keys of this maps
   * are the locations of the servers. The values are vectors containing the elements
   * hashed there.
   *
   * This is implemented as a red-black tree, with amortized O(log n) 
   *     insertions, deletions, and lookups.
   */
  MapType cache_indices_;

  /**
   * This is the key space size. All server ids are between 0 and kss_
   */
  long long kss_;

  /**
   * Track the numbers of servers and keys
   */
  int num_servers_;
  int num_keys_;
  std::function<long long(long long, long long)> hash;
public:
  friend class CuckooRings;
 /**
   * Constructor for the RingHash function.
   * @param key_space_size coreesponds to the maximum server id
   * @param init_servers number of servers to initialize the ring with
   */
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

  /**
   * Constructor for the RingHash function.
   * @param key_space_size coreesponds to the maximum server id
   * @param init_servers number of servers to initialize the ring with
   * @param hashfn the hash function to be used by this ring
   */
  RingHash(long long key_space_size, long long init_servers,
      std::function<long long(long long, long long)> hashfn) :
      kss_(key_space_size), num_servers_(init_servers) {
    num_keys_ = 0;
    // Set up keyspace now
    for (int i = 0; i < init_servers; ++i) {
      cache_indices_.insert(std::make_pair( (long long) (i* (((double)key_space_size)/init_servers)),
          std::vector<int>()));
    }
    hash = hashfn;
  }

  /**
   * Destructor
   */
  ~RingHash() {}

  /**
   * @brief Determines the number of servers in the Ring
   * @returns int indicating how many servers there are
   */
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
    MapIterator m = cache_indices_.lower_bound(tmp);
    if (m == cache_indices_.end()){
      m = cache_indices_.begin();

    }
    return m->first;
  }


  /**
   * #param location where server will be put
   * @brief adds a server to the RingHash
   * @returns void
   */
  void add_server(int server_loc) {
    
    VectorIterator it;
    MapIterator m = cache_indices_.lower_bound(server_loc);
    if (m == cache_indices_.end()){
      m = cache_indices_.begin();
    }
    server_id server_to_bump = m->first;
    // note that this works since we are putting the
    // new server at the location of its hash

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
    
  /**
   * @brief removes all of the jobs given to a server
   * @returns void
   */
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

  /**
   * @brief Iterates through the servers and finds the max load
   * @returns long long representing the max load
   */
  long long get_max_load(void) {
    int sz = 0;
    for (const auto&x : cache_indices_) {
      if (x.second.size() > sz) {
        sz = x.second.size();
      }
    }
    return sz;
  }

  /**
   * @brief Iterates through the servers and finds the min load
   * @returns long long representing the min load
   */
  long long get_min_load(void) {
    long long sz = LLONG_MAX;
    for (const auto&x : cache_indices_) {
      if (x.second.size() < sz) {
        sz = x.second.size();
      }
    }
    return sz;
  }  

  /**
   * @brief Determines the average load across the servers
   * @returns float representing the average load
   */
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

  /**
   * @brief Determines the variance of the loads
   * @returns long long representing the variance
   */
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
  /**
   * @brief, picks a server at random and removes it by calling remove_server
   * @returns void
   */
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
  /**
   * @brief The penalty for a particular bin's load
   * @returns long long representing this penalty
   */
  long long costfunction(int sz) {
    return pow(sz, 2.0);
  }
  // Determine the cost of the ring (for example, having a squared penalty based on load)
  /**
   * @brief Uses the costfunction defined above to determine the total cost of the RingHash
   * @return long long of the total cost
   */ 
  long long cost_of_structure(void){
    long long cost = 0;
    for (const auto&x : cache_indices_) {
      cost += costfunction(x.second.size());
    }
    return cost;
  }

  /**
   * @brief determines the number of keys/jobs in the RIng
   * @returns long long representing the number of keys
   */
  long long getNumKeys(void){
    long long tot = 0;
    for (const auto&x : cache_indices_) {
      tot += x.second.size();
    }
    return tot;
  }

  /**
   * @brief determines the number of servers in the RingHash
   * @returns long long of the number of servers
   */
  long long getNumServers(void){
    long long tot = 0;
    for (const auto&x : cache_indices_) {
      tot += 1;
    }
    return tot;
  }

  /**
   * @brief adds a server to a random (unused) location in the HashRing
   * @returns void
   */
  void add_random_server(void){
    int randnum = 0;

    // make sure that the server is not already in there.
    do {
      randnum = rand();
    } while (cache_indices_.find(randnum) != cache_indices_.end());

    add_server(randnum);
  }

  /**
   * @brief Gets the keys of a particular server
   * @returns vector<int> of keys
   */
  vector<int>& get_keys(server_id s) {
    return cache_indices_[s];
  }

};
