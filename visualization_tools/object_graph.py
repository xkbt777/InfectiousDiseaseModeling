import matplotlib.pyplot as plt
import sys
from matplotlib.patches import Rectangle

objects = []
with open(sys.argv[1], "r") as file:
    matrix_size = int(file.readline())
    for line in file:
        objects.append(line.rstrip().split(","))

plt.axis([0, matrix_size, 0, matrix_size])
ax = plt.gca()

for object in objects:
    color = 'r'
    if int(object[5]) == 1:
        color = 'b'
    elif int(object[5]) == 2:
        color = 'g'
    ax.add_patch(Rectangle((float(object[1]), float(object[2])), float(object[3]) - float(object[1]), float(object[4]) - float(object[2]), edgecolor=color, facecolor='none'))
    cx = (float(object[1]) + float(object[3])) / 2.0
    cy = (float(object[2]) + float(object[4])) / 2.0
    ax.annotate(object[0], (cx, cy), color=color, weight='bold', fontsize=6, ha='center', va='center')

plt.show()
