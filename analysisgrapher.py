# Script to analyze and graph runtimes from cuckoorings

import matplotlib.pyplot as plt

output = open("timingoutput")

ringtimes = []
cuckootimes = []

i = 0
for l in output:
  i += 1
  if (i % 2):
    ringtimes.append(float(l))
  else:
    cuckootimes.append(float(l))

assert(len(ringtimes) == len(cuckootimes))

plt.gca().set_color_cycle(['red', 'blue'])

plt.plot(range(len(ringtimes)), ringtimes)
plt.plot(range(len(cuckootimes)), cuckootimes)

plt.legend(['Standard Consistent Hashing', 'CuckooRings'], loc = 'upper left')
plt.title('Time Comparison of Consistent Hashing and CuckooRings')

plt.show()
