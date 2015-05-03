# Script to analyze and graph runtimes from cuckoorings

import matplotlib.pyplot as plt

output = open("timingoutput")

ringtimes = []
cuckootimes = []
ringcost = []
cuckoocost = []
ringmax = []
cuckoomax = []

i = 0
for l in output:
	line = l.split(",")
	line = [float(i) for i in line]
  i += 1
  if (i % 2):
    ringtimes.append(line[0])
    ringcost.append(line[1])
    ringmax.append(line[2])
  else:
    cuckootimes.append(line[0])
    cuckoocost.append(line[1])
    cuckoomax.append(line[2])


assert(len(ringtimes) == len(cuckootimes))

plt.gca().set_color_cycle(['red', 'blue'])
plt.figure(1)
plt.plot([50000*(i+1) for i in range(20)], ringtimes)
plt.plot([50000*(i+1) for i in range(20)], cuckootimes)

plt.legend(['Standard Consistent Hashing', 'CuckooRings'], loc = 'upper left')
plt.title('Time Comparison of Consistent Hashing and CuckooRings')

plt.figure(2)
plt.plot([50000*(i+1) for i in range(20)], ringcost)
plt.plot([50000*(i+1) for i in range(20)], cuckoocost)

plt.legend(['Standard Consistent Hashing', 'CuckooRings'], loc = 'upper left')
plt.title('Cost Comparison of Consistent Hashing and CuckooRings')

plt.figure(3)
plt.plot([50000*(i+1) for i in range(20)], ringmax)
plt.plot([50000*(i+1) for i in range(20)], cuckoomax)

plt.legend(['Standard Consistent Hashing', 'CuckooRings'], loc = 'upper left')
plt.title('Max Load Comparison of Consistent Hashing and CuckooRings')

plt.show()
