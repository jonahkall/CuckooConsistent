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

  // repeat the experiment several times
  for (int repeat = 1; repeat < 4; ++repeat) {
    for (j = 100000; j <= 2000000; j += 100000) {

      // initialize the data structures
      RingHash r((1L << 32), 1000000);
      CuckooRings c((1L << 32), 500000);

      // begin timing the RingHash
      t1 = clock();

      // make the insertions
      for (i = 1; i <= j; ++i) {
        r.insert(i);
      }
      t2 = clock();

      // print statistics
      cout << ((float)(t2-t1))/CLOCKS_PER_SEC << ",";
      cout << ((float) r.cost_of_structure()) / r.getNumServers() << ",";
      cout << r.get_max_load() << endl;

      // reset the clock
      t1 = clock();

      // make the insertions into the CuckooRings
      for (i = 1; i <= j; ++i) {
        c.insert(i);
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
