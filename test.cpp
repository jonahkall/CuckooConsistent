// set::lower_bound/upper_bound
#include <iostream>
#include <string>
#include <set>
#include <functional>
#include <map>
#include <time.h>
#include <assert.h>
#include <stdlib.h>

#include "cuckoorings.hpp"

using namespace std;

int main ()        
{
  clock_t t1, t2, t3;
  // // initialize ringhash
  // RingHash r((1L << 32), 100);
  // r.print_loads();


  // // add a bunch of values to it
  // for (int i = 1; i < 1000; ++i)
  // 	r.insert(i);

  // for (int i = 1; i < 50000; ++i){
  //   unsigned int first = r.lookup(i);
  //   unsigned int second = r.lookup(i);
  //   assert(first == second);
  // }

  // r.print_loads();

  // // add a few servers to it
  // for (long long i = 1; i < 1000000000; i += 100000000){
  //   //cout << i << endl;
  // 	r.add_server(i);
  // }

  // r.print_loads();

  // // remove a bunch of servers
  // for (int i = 1; i < 1000; ++i){
  //   r.remove_server(r.lookup(i));
  //   //cout << "Worst server is: " << r.get_max_load_server() << endl;
  // }

  // r.print_loads();
  // cout << endl;
  // cout << endl;
  srand (time(NULL));


  t1 = clock();
  cout << "\nTesting a standard ring" << endl;
  
  RingHash r((1L << 32), 1000000);

  // add 500K, then remove 2k, then add 500k
  cout << "    adding 500,000 to the ring" << endl;
  for (int i = 1; i < 500000; ++i) {
    r.insert(i);
  }

  cout << "    randomly removing 2000 servers" << endl;
  for (int rem = 0; rem < 2000; ++rem){
    r.remove_random_server();
  }
  cout << "    adding 500,000 to the ring" << endl;
  for (int i = 1; i < 500000; ++i) {
    r.insert(i);
  }

  t2 = clock();
  cout << "\nTesting CuckooRings" << endl;
  
  // add 500k, then remove 2k, then add 500k
  CuckooRings c((1L << 32), 500000);
  cout << "    adding 500,000 to the CuckooRings" << endl;
  for (int i = 1; i < 500000; ++i)
    c.insert(i);

  cout << "Before removing any, we had: " << c.getNumServers() << " servers\n";

  cout << "    randomly removing 2000 servers (1000 from each Ring)" << endl;
  for (int rem = 0; rem < 1000; ++rem){
    c.remove_random_server(0, 0);
  }
  for (int rem = 0; rem < 1000; ++rem){
    c.remove_random_server(0, 1);
  }
  cout << "    adding 500,000 to the CuckooRings\n" << endl;
  for (int i = 1; i < 500000; ++i)
    c.insert(i);

  t3 = clock();

  cout << "Standard Ring Results" << endl;
  cout << "    Total load is: " << r.getNumKeys() << endl;
  cout << "    Number of servers is: " << r.getNumServers() << endl;
  cout << "    Max load is: " << r.get_max_load() << endl;
  cout << "    Variance of loads is:" << r.get_variance_load() << endl;
  cout << "    Time elapsed (sec): " << ((float) (t2 - t1))/CLOCKS_PER_SEC << endl; 
  cout << "    Cost: " << r.cost_of_structure() << endl << endl;


  cout << "Cuckoo Ring Results" << endl;
  cout << "    Total load is: " << c.getNumKeys() << endl;
  cout << "    Number of servers is: " << c.getNumServers() << endl;
  cout << "    Max load is: " << c.get_max_load() << endl;
  cout << "    Time elapsed (sec): " << ((float) (t3 - t2))/CLOCKS_PER_SEC << endl; 
  cout << "    Cost: " << c.cost_of_structure() << endl << endl; 
  


  


  //c.print_loads();

  //c.send_server_rtol(c.get_max_load_server());

  //cout << "Loads after sending a server from right to left are: \n\n";

  return 0;
}
