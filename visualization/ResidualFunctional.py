import matplotlib.pyplot as plt
import numpy as np
import sys
import os
path = os.path.abspath(sys.argv[0])
path = path[:path.rfind("\\")]
path = path[:path.rfind("\\")]

plt.figure(figsize=(14, 8))

graphRf = np.genfromtxt(path + "/res/output/residual functional.txt")
Rf = plt.plot(graphRf[:, 0], graphRf[:, 1], 'o-') 
plt.setp(Rf, color='red')
plt.tick_params(axis = 'both', which = 'major', labelsize = 12) 
plt.xlabel('iter', fontsize=18) 
plt.ylabel('functional value', fontsize=18) 
plt.legend(['Residual functional'], fontsize=14)
plt.grid(color = 'grey', linewidth = 1)
plt.savefig(path + "/res/graph/residualFunctional.png", bbox_inches='tight') 
#plt.show()
