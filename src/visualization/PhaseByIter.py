import matplotlib.pyplot as plt
import numpy as np

plt.figure(figsize=(22, 12))

plt.subplot(121)
plt.title(label='a', loc='center', fontsize=24, pad = 20)
graphPw = np.genfromtxt("../../res/output/Produced water.txt")
Pw = plt.plot(graphPw[:, 0], graphPw[:, 1], 'o-') 
plt.setp(Pw, color='blue')
plt.tick_params(axis = 'both', which = 'major', labelsize = 16) 
plt.xlabel('iter', fontsize=28) 
plt.ylabel('Volume water, m3', fontsize=28) 
plt.legend(['Produced water'], fontsize=24)
plt.grid(color = 'grey', linewidth = 1)


plt.subplot(122)
plt.title(label='b', loc='center', fontsize=24, pad = 20)
graphPo = np.genfromtxt("../../res/output/Produced oil.txt")
Po = plt.plot(graphPo[:, 0], graphPo[:, 1], 'o-')
plt.setp(Po, color='black')
plt.tick_params(axis = 'both', which = 'major', labelsize = 16) 
plt.xlabel('iter', fontsize=28)  
plt.ylabel('Volume oil, m3', fontsize=28) 
plt.legend(['Produced oil'], fontsize=24)
plt.grid(color = 'grey', linewidth = 1)

plt.savefig('../../res/graph/PhaseByIter.png', bbox_inches='tight') 
plt.show()
