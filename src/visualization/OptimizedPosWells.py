import matplotlib.pyplot as plt
import numpy as np
import matplotlib.patches
from matplotlib.ticker import FormatStrFormatter
import matplotlib.ticker as ticker

def drawPolygons(axes, array):
    polygon_2 = matplotlib.patches.Polygon(array, fill=False, color = 'black', linewidth= 10)
    axes.add_patch(polygon_2) 

if __name__ == "__main__":

    fig, ax = plt.subplots(figsize=(22, 22)) 

    Rf = np.genfromtxt("../../res/output/Residual functional.txt")
    val, idx = min((val, idx) for (idx, val) in enumerate(Rf[:, 1]))
    indexMinResidualFunctional = idx
    
    gridx = np.genfromtxt("../../res/models/goodModel0/mesh/x.txt")
    gridy = np.genfromtxt("../../res/models/goodModel0/mesh/y.txt")
    
    for i in range(1, len(gridx) - 1, 1): 
        plt.vlines(x = [gridx[i], gridx[i + 1]], ymin = gridy[1], ymax = gridy[len(gridy) - 1], color = 'grey', linewidth= 1)
        plt.hlines(y = [gridy[i], gridy[i + 1]], xmin = gridx[1], xmax = gridx[len(gridx) - 1], color = 'grey', linewidth= 1)  
    
    #plt.xticks(ticks = gridx[1:])
    #plt.yticks(ticks = gridy[1:])   
    ax.tick_params(axis = 'both', which = 'major', labelsize = 16)    
    #ax.yaxis.set_major_formatter(FormatStrFormatter('%.0f'))
    #ax.xaxis.set_major_formatter(FormatStrFormatter('%.0f'))   
    #plt.grid(which='major', color = 'grey', linewidth = 1)
    
    #array = [(-498, -3), (-275, -3), (-273, 61), (-135, 62), (-134, 197), (-257, 200), (-495, 197)] 
    array = [(-917, -530), (-501, -949), (491, -949), (894, -619), (1040, 91), (922, 592), (507, 954), (-349, 954), (-834, 706), (-1034, 110)] 
    drawPolygons(ax, array)
    
    colors = ['b', 'g', 'r', 'c', 'm', 'y', 'rosybrown','firebrick','darksalmon','sienna','sandybrown','tan','gold','darkkhaki','olivedrab','chartreuse','palegreen','darkgreen','darkcyan','deepskyblue','royalblue','navy','mediumpurple','darkochid','plum']
    typeWell = np.genfromtxt("../../res/input/well task parameters.txt", skip_header = 1)  
    allowedValues = np.genfromtxt("../../res/output/Allowed values.txt")  
    numberWells = int(len(allowedValues)/2)
    for i in range(0, len(allowedValues), 2):        
        plt.vlines(x = [allowedValues[i][0], allowedValues[i][1]], ymin = allowedValues[i + 1][0], ymax = allowedValues[i + 1][1], color = colors[int(i/2)], linewidth= 8, linestyle='--')
        plt.hlines(y = [allowedValues[i + 1][0], allowedValues[i + 1][1]], xmin = allowedValues[i][0], xmax = allowedValues[i][1], color = colors[int(i/2)], linewidth= 8, linestyle='--')   
     
    wellCenters = np.genfromtxt("../../res/output/Well centers.txt")   
       
    graphsx = [[0] * int(len(wellCenters)/numberWells) for i in range(numberWells)]
    graphsy = [[0] * int(len(wellCenters)/numberWells) for i in range(numberWells)]
    
    
    for j in range(0, numberWells):          
        graphsx[j][0] = wellCenters[numberWells*indexMinResidualFunctional+j, 0]
        graphsy[j][0] = wellCenters[numberWells*indexMinResidualFunctional+j, 1]   
       
    for j in range(0, numberWells):
        plt.annotate('Well'+ str(j+1), xy=(graphsx[j][0], graphsy[j][0]), xycoords='data', xytext=(graphsx[j][0], graphsy[j][0]+5), textcoords='data', fontsize=28)       
        
    for j in range(0, numberWells):
        g = plt.plot(graphsx[j][0], graphsy[j][0], '-', label='Well'+ str(j+1))        
        plt.setp(g, color=colors[j], linewidth= 2) 
        if typeWell[4 * j][0] == 1:    
            g = plt.scatter(graphsx[j][0], graphsy[j][0], marker="o", s = 350)        
            plt.setp(g, color=colors[j]) 
        else :   
            g = plt.scatter(graphsx[j][0], graphsy[j][0], marker="^", s = 350)        
            plt.setp(g, color=colors[j]) 
    
    ax.margins(x=-0.3, y=-0.3)
    plt.xlabel('x, m', fontsize=32)  
    plt.ylabel('y, m', fontsize=32) 
    
    plt.savefig('../../res/graph/OptimizedPosWells.png', bbox_inches='tight')            
    plt.show() 