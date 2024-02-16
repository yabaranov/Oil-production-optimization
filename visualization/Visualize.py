import matplotlib.pyplot as plt
import numpy as np
import matplotlib.patches
import json
import sys
import os
path = os.path.abspath(sys.argv[0])
path = path[:path.rfind("\\")]
path = path[:path.rfind("\\")]

def drawPolygons(axes, array):
    polygon_2 = matplotlib.patches.Polygon(array, fill=False, color = 'black', linewidth= 10)
    axes.add_patch(polygon_2)  


fig, ax = plt.subplots(figsize=(22, 22))  
gridx = np.genfromtxt(path + "/res/models/model0/mesh/x.txt")
gridy = np.genfromtxt(path + "/res/models/model0/mesh/y.txt")

for i in range(1, len(gridx) - 1, 1): 
    plt.vlines(x = [gridx[i], gridx[i + 1]], ymin = gridy[1], ymax = gridy[len(gridy) - 1], color = 'grey', linewidth= 1)
    plt.hlines(y = [gridy[i], gridy[i + 1]], xmin = gridx[1], xmax = gridx[len(gridx) - 1], color = 'grey', linewidth= 1)   
 
ax.tick_params(axis = 'both', which = 'major', labelsize = 16)    

contours = []

with open(path + "/res/models/model0/mesh/ContoursSaturationGeometry", 'r') as f:
    for i in range(3):
        f.readline()
    numberContours = int(f.readline())
    for i in range(numberContours):
        numberPoints = int(f.readline())
        contour = []
        for j in range(numberPoints):
            (x,y) = f.readline().split()
            contour.append((x, y))
    contours.append(contour)


for i in range(len(contours)):
    drawPolygons(ax, contours[i])
 
with open(path + "/res/input/well parameters.json", "r") as f:
    wellParameters = json.load(f)   
                
allowedValues = np.genfromtxt(path + "/res/output/allowed values.txt")  
numberWells = int(len(allowedValues)/2)
   
colors = ['b', 'g', 'r', 'c', 'm', 'y', 'rosybrown','firebrick','darksalmon','sienna','sandybrown','tan','gold',
'darkkhaki','olivedrab','chartreuse','palegreen','darkgreen','darkcyan','deepskyblue','royalblue','navy','mediumpurple','darkochid','plum']

for i in range(0, len(allowedValues), 2):        
    plt.vlines(x = [allowedValues[i][0], allowedValues[i][1]], ymin = allowedValues[i + 1][0], ymax = allowedValues[i + 1][1], color = colors[int(i/2)], linewidth= 8,linestyle='--')
    plt.hlines(y = [allowedValues[i + 1][0], allowedValues[i + 1][1]], xmin = allowedValues[i][0], xmax = allowedValues[i][1], color = colors[int(i/2)], linewidth= 8,linestyle='--')   
 
wellCenters = np.genfromtxt(path + "/res/output/well centers.txt")   
  
graphsx = [[0] * int(len(wellCenters)/numberWells) for i in range(numberWells)]
graphsy = [[0] * int(len(wellCenters)/numberWells) for i in range(numberWells)]

for i in range(0, len(wellCenters), numberWells):
    for j in range(0, numberWells):          
        graphsx[j][int(i/numberWells)] = wellCenters[i+j, 0]
        graphsy[j][int(i/numberWells)] = wellCenters[i+j, 1]   

for j in range(0, numberWells):
    plt.annotate('begin', xy=(graphsx[j][0], graphsy[j][0]), xycoords='data', xytext=(graphsx[j][0], graphsy[j][0]+5), textcoords='data', fontsize=24)
    plt.annotate('end', xy=(graphsx[j][int(len(wellCenters)/numberWells)-1], graphsy[j][int(len(wellCenters)/numberWells)-1]), xycoords='data', xytext=(graphsx[j][int(len(wellCenters)/numberWells)-1], graphsy[j][int(len(wellCenters)/numberWells)-1]-10), textcoords='data', fontsize=24)
    
for j in range(0, numberWells):
    g = plt.plot(graphsx[j], graphsy[j], '-', label='Well'+ str(j + 1))        
    plt.setp(g, color=colors[j], linewidth= 2) 
    if wellParameters["wellsParameters"][j]["type"] == "Production":    
        g = plt.scatter(graphsx[j], graphsy[j], marker="o", s = 100)        
        plt.setp(g, color=colors[j], linewidth= 1) 
    else :   
        g = plt.scatter(graphsx[j], graphsy[j], marker="^", s = 100)        
        plt.setp(g, color=colors[j], linewidth= 1)

ax.margins(x=-0.14, y=-0.14)    
plt.xlabel('x, m', fontsize=32)  
plt.ylabel('y, m', fontsize=32)         
plt.legend(fontsize=24, loc = 'upper right') 
plt.savefig(path + "/res/graph/visualize.png", bbox_inches='tight')     
#plt.show() 