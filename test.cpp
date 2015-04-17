// set::lower_bound/upper_bound
#include <iostream>
#include "ringhash.hpp"
#include <string>
#include <set>
#include <functional>
#include <map>

using namespace std;

int main ()        
{
  RingHash r((1L << 32), 100);
  r.print_loads();
  cout << endl;
  for (int i = 1; i < 100; ++i)
  	r.insert(i);
  r.print_loads();
  cout << endl;
  

  for (int i = 1; i < 100; ++i)
  	r.add_server(i);
  r.print_loads();
  cout << endl;

  for (int i = 1; i < 100; ++i)
  	r.remove_server(r.lookup(i));
  
   r.print_loads();
  cout << endl;
  return 0;
}