// set::lower_bound/upper_bound
#include <iostream>
#include "ringhash.h"
#include <string>
#include <set>
#include <functional>
#include <map>

using namespace std;

int main ()        
{
  RingHash r((1L << 32), 100);
  r.print_loads();
  return 0;
}