/*
 *  Tests speed benefits of a random set of insertions, along with server removals
 *  Increment j in steps to 50,000 up to 1,000,000. For each j, make Ring and CuckooRing of that many servers.
 *  For j steps, add items to the servers, and with small probability remove a server.
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
  clock_t t1, t2, t3;
  srand (time(NULL));

  int i, j;
  for (j = 0; j <= 1000000; j += 50000) {
    RingHash r((1L << 32), j);
    CuckooRings c((1L << 32), j/2);
    t1 = clock();

    for (i = 0; i <= j; ++i) {
      int randnum = rand();
      double n = ((double) randnum / (RAND_MAX));
      if (n < .1){
        r.remove_random_server();
      }
      else if (n < .2){
        r.add_random_server();
      }

      r.insert(i);
    }

    t2 = clock();
    cout << ((float)(t2-t1))/CLOCKS_PER_SEC << ",";
    cout << ((float) r.cost_of_structure()) / r.getNumServers() << ",";
    cout << r.get_max_load() << endl;
    t1 = clock();
    for (i = 0; i <= j; ++i) {
      int randnum = rand();
      double n = ((double) randnum / (RAND_MAX));
      if (n < .1){
        c.remove_random_server(0, (randnum % 2));
      }
      else if (n < .2){
        c.add_random_server(randnum % 2);
      }
      c.insert(i);
    }
    t2 = clock();

    cout << ((float)(t2-t1))/CLOCKS_PER_SEC << ",";
    cout << ((float) c.cost_of_structure()) / c.getNumServers() << ",";
    cout << c.get_max_load() << endl;
  }

  return 0;
}
