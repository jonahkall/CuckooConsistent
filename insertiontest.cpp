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
  for (j = 50000; j <= 1000000; j += 25000) {
    RingHash r((1L << 32), 1000000);
    CuckooRings c((1L << 32), 500000);
    t1 = clock();
    for (i = 1; i <= j; ++i) {
      r.insert(i);
    }
    t2 = clock();
    cout << ((float)(t2-t1))/CLOCKS_PER_SEC << endl;
    t1 = clock();
    for (i = 1; i <= j; ++i) {
      c.insert(i);
    }
    t2 = clock();
    cout << ((float)(t2-t1))/CLOCKS_PER_SEC << endl;
  }

  return 0;
}
