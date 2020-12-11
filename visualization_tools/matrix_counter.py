import matplotlib.pyplot as plt
import sys
from matplotlib.patches import Rectangle

objects = []
with open("assignment", "r") as file:
    matrix_size = float(file.readline())
    for line in file:
        objects.append(line.rstrip().split(","))

plt.axis([0, matrix_size, 0, matrix_size])
ax = plt.gca()

for object in objects:
    ax.add_patch(Rectangle((float(object[3]), float(object[4])), float(object[5]) - float(object[3]), float(object[6]) - float(object[4]), edgecolor='g', facecolor='none'))
    cx = (float(object[3]) + float(object[5])) / 2.0
    cy = (float(object[4]) + float(object[6])) / 2.0
    ax.annotate(object[1], (cx, cy), weight='bold', fontsize=6, ha='center', va='center')

plt.show()
