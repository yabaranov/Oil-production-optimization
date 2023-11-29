import matplotlib.pyplot as plt
import numpy as np
import sys

plt.figure(figsize=(12, 8))

graphPo = np.genfromtxt("../../res/output/" + sys.argv[1] + ".txt")
Po = plt.plot(graphPo[:, 0], graphPo[:, 1], 'o-')
plt.setp(Po, color='black')
plt.xlabel('iter', fontsize=16)  
plt.ylabel('functional value', fontsize=16) 
plt.legend([sys.argv[1]], fontsize=16)
plt.grid(which='major', color = 'grey', linewidth = 1)

plt.savefig('../../res/graph/' + sys.argv[1] + '.png', bbox_inches='tight') 
plt.show()