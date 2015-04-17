// set::lower_bound/upper_bound
#include <iostream>
#include "ringhash.hpp"
#include <string>
#include <set>
#include <functional>
#include <map>
#include <assert.h>

using namespace std;

int main ()        
{
  // initialize ringhash
  RingHash r((1L << 32), 100);
  r.print_loads();
  cout << endl;
  cout << endl;

  // add a bunch of values to it
  for (int i = 1; i < 100; ++i)
  	r.insert(i);

  //r.print_loads();

  for (int i = 1; i < 50000; ++i){
    unsigned int first = r.lookup(i);
    unsigned int second = r.lookup(i);
    assert(first == second);
    //cout << first << endl;

  }

  r.print_loads();





  cout << endl;
  cout << endl;
  
  // add a few servers to it
  for (long long i = 1; i < 1000000000; i += 100000000){
    //cout << i << endl;
  	r.add_server(i);
  }

  r.print_loads();
  cout << endl;
  cout << endl;

  // remove a bunch of servers
  for (int i = 1; i < 100; ++i)
  	r.remove_server(r.lookup(i));

  cout << endl;

  r.print_loads();
  cout << endl;
  cout << endl;
  return 0;
}