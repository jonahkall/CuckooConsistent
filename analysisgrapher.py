# Script to analyze and graph runtimes from cuckoorings

import matplotlib.pyplot as plt
import sys

output = open(sys.argv[1])

size = 3
numloops = 20

ringtimes = [0]*numloops
cuckootimes = [0]*numloops
ringcost = [0]*numloops
cuckoocost = [0]*numloops
ringmax = [0]*numloops
cuckoomax = [0]*numloops

i = 0
for l in output:
  line = l.split(",")
  line = [float(j) for j in line]
  i += 1
  if (i % 2):
    ringtimes[i/2 % numloops] += line[0]
    ringcost[i/2 % numloops] += line[1]
    ringmax[i/2 % numloops] += line[2]
  else:
    cuckootimes[(i-1)/2 % numloops] += line[0]
    cuckoocost[(i-1)/2 % numloops] += line[1]
    cuckoomax[(i-1)/2 % numloops] += line[2]

ringtimes = [i/size for i in ringtimes]
cuckootimes = [i/size for i in cuckootimes]
ringcost = [i/size for i in ringcost]
ringmax = [i/size for i in ringmax]
cuckoocost = [i/size for i in cuckoocost]
cuckoomax = [i/size for i in cuckoomax]

assert(len(ringtimes) == len(cuckootimes))

plt.gca().set_color_cycle(['red', 'blue'])
plt.figure(1)
plt.xlabel('Number of servers removed')
plt.ylabel('Time (s)')
plt.axis([0,101000,0,3.2])
plt.plot([5000*(i) for i in range(20)], ringtimes)
plt.plot([5000*(i) for i in range(20)], cuckootimes)

plt.legend(['Standard Consistent Hashing', 'CuckooRings'], loc = 'upper left')
plt.title('Time Comparison of Consistent Hashing and CuckooRings')

plt.gca().set_color_cycle(['red', 'blue'])
plt.figure(2)
plt.xlabel('Number of servers removed')
plt.ylabel('Cost (average squared load)')
plt.axis([0,101000,0,4])
plt.plot([5000*(i) for i in range(20)], ringcost)
plt.plot([5000*(i) for i in range(20)], cuckoocost)

plt.legend(['Standard Consistent Hashing', 'CuckooRings'], loc = 'upper left')
plt.title('Cost Comparison of Consistent Hashing and CuckooRings')

plt.gca().set_color_cycle(['red', 'blue'])
plt.figure(3)
plt.xlabel('Number of servers removed')
plt.ylabel('Max load')
plt.axis([0,101000,0,30])
plt.scatter([5000*(i) for i in range(20)], ringmax, c='red')
plt.scatter([5000*(i) for i in range(20)], cuckoomax)

plt.legend(['Standard Consistent Hashing', 'CuckooRings'], loc = 'upper left')
plt.title('Max Load Comparison of Consistent Hashing and CuckooRings')

for im in plt.gca().get_images():
  im.set_color_cycle(['red', 'blue'])

plt.show()
