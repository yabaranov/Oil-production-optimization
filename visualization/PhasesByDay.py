import matplotlib.pyplot as plt
import numpy as np
import sys
import os
path = os.path.abspath(sys.argv[0])
path = path[:path.rfind("\\")]
path = path[:path.rfind("\\")]

plt.figure(figsize=(22, 12))

Rf = np.genfromtxt(path + "/res/output/Residual functional.txt")
val, idx = min((val, idx) for (idx, val) in enumerate(Rf[:, 1]))
indexMinResidualFunctional = idx

plt.subplot(121)
plt.title(label='a', loc='center', fontsize=24, pad = 20)
initial = np.genfromtxt(path + "/res/output/production by day/produced water by day Well" + sys.argv[1] + " iteration1.txt")
optimized = np.genfromtxt(path + "/res/output/production by day/produced water by day Well" + sys.argv[1] + ' iteration' + str(indexMinResidualFunctional) + ".txt")

i = plt.plot(initial[:, 0], initial[:, 1], 's-', label='initial '+ 'Well'+ sys.argv[1])        
plt.setp(i, color='aqua') 

o = plt.plot(optimized[:, 0], optimized[:, 1], 'o-', label='optimized '+ 'Well'+ sys.argv[1])        
plt.setp(o, color='red') 

plt.tick_params(axis = 'both', which = 'major', labelsize = 16) 
plt.xlabel('t(day)', fontsize=28) 
plt.ylabel('Volume water, m3', fontsize=28) 
plt.legend(fontsize=24)
plt.grid(color = 'grey', linewidth = 1)

plt.subplot(122)
plt.title(label='b', loc='center', fontsize=24, pad = 20)
initial = np.genfromtxt(path + "/res/output/production by day/produced oil by day Well" + sys.argv[1] + " iteration1.txt")
optimized = np.genfromtxt(path + "/res/output/production by day/produced oil by day Well" + sys.argv[1] + ' iteration' + str(indexMinResidualFunctional) + ".txt")

i = plt.plot(initial[:, 0], initial[:, 1], 's-', label='initial '+ 'Well'+ sys.argv[1])        
plt.setp(i, color='aqua') 

o = plt.plot(optimized[:, 0], optimized[:, 1], 'o-', label='optimized '+ 'Well'+ sys.argv[1])        
plt.setp(o, color='red') 

plt.tick_params(axis = 'both', which = 'major', labelsize = 16) 
plt.xlabel('t(day)', fontsize=28) 
plt.ylabel('Volume oil, m3', fontsize=28) 
plt.legend(fontsize=24)
plt.grid(color = 'grey', linewidth = 1)

plt.savefig(path + "/res/graph/phasesByDayWell" + sys.argv[1] + '.png', bbox_inches='tight') 
#plt.show()

