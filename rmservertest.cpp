/*
 *  Tests speed benefits of removing a server. 
 *  Begin by adding 1000000 items to 1000000 servers. Then remove 0 to 100000 servers (10%) by increments of 5000
 *  and track the time it takes to do that for CuckooRing and regular Ring.
 */

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
  // initialize variables and seed the random number generator
  clock_t t1, t2, t3;
  srand (time(NULL));
  int i, j;

  // repeat the test several times
  for (int repeat = 1; repeat < 4; ++repeat) {

    for (j = 0; j <= 100000; j += 5000) {

      // initialize the data structures
      RingHash r((1L << 32), 1000000);
      CuckooRings c((1L << 32), 500000);

      // begin timing the RingHash
      t1 = clock();

      // make all of the insertions
      for (i = 1; i <= 1000000; ++i) {
        r.insert(i);
      }

      // remove some number of servers randomly
      for (i = 0; i <= j; ++i) {
        r.remove_random_server();
      }

      t2 = clock();

      // print statistics
      cout << ((float)(t2-t1))/CLOCKS_PER_SEC << ",";
      cout << ((float) r.cost_of_structure()) / r.getNumServers() << ",";
      cout << r.get_max_load() << endl;

      // reset the clock
      t1 = clock();

      // insert keys into CuckooRings
      for (i = 1; i <= 1000000; ++i) {
        c.insert(i);
      }

      // remove servers randomly from each side
      for (i = 0; i <= j/2; ++i) {
        c.remove_random_server(0, 0);
        c.remove_random_server(0, 1);
      }
      t2 = clock();

      // print statistics
      cout << ((float)(t2-t1))/CLOCKS_PER_SEC << ",";
      cout << ((float) c.cost_of_structure()) / c.getNumServers() << ",";
      cout << c.get_max_load() << endl;  
    }
  }

  return 0;
}
