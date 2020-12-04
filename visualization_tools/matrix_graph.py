import matplotlib.pyplot as plt
import sys
from matplotlib.patches import Rectangle

objects = []
with open("matrix", "r") as file:
    matrix_size = float(file.readline())
    for line in file:
        objects.append(line.rstrip().split(","))

plt.axis([0, matrix_size, 0, matrix_size])
ax = plt.gca()

for object in objects:
    ax.add_patch(Rectangle((float(object[1]), float(object[2])), float(object[3]) - float(object[1]), float(object[4]) - float(object[2]), edgecolor='g', facecolor='none'))
    cx = (float(object[1]) + float(object[3])) / 2.0
    cy = (float(object[2]) + float(object[4])) / 2.0
    ax.annotate(object[0], (cx, cy), weight='bold', fontsize=6, ha='center', va='center')

plt.show()
