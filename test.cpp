// set::lower_bound/upper_bound
#include <iostream>
#include <string>
#include <set>
#include <functional>
#include <map>
#include <assert.h>

#include "cuckoorings.hpp"

using namespace std;

int main ()        
{
  // initialize ringhash
  RingHash r((1L << 32), 100);
  r.print_loads();


  // add a bunch of values to it
  for (int i = 1; i < 1000; ++i)
  	r.insert(i);

  for (int i = 1; i < 50000; ++i){
    unsigned int first = r.lookup(i);
    unsigned int second = r.lookup(i);
    assert(first == second);
  }

  r.print_loads();

  // add a few servers to it
  for (long long i = 1; i < 1000000000; i += 100000000){
    //cout << i << endl;
  	r.add_server(i);
  }

  r.print_loads();

  // remove a bunch of servers
  for (int i = 1; i < 100; ++i){
    r.remove_server(r.lookup(i));
    //cout << "Worst server is: " << r.get_max_load_server() << endl;
  }

  r.print_loads();
  cout << endl;
  cout << endl;

  cout << "Beginning CuckooRings testing proper\n\n\n";

  CuckooRings c((1L << 32), 200);
  for (int i = 100; i < 10000; ++i)
    c.insert(i);

  c.print_loads();

  c.send_server_rtol(c.get_max_load_server());

  cout << "Loads after sending a server from right to left are: \n\n";

  c.print_loads();


  return 0;
}
