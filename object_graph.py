import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

objects = []
with open("./objects", "r") as file:
    matrix_size = int(file.readline())
    for line in file:
        objects.append(line.rstrip().split(","))

plt.axis([0, matrix_size, 0, matrix_size])
ax = plt.gca()

for object in objects:
    ax.add_patch(Rectangle((float(object[1]), float(object[2])), float(object[3]) - float(object[1]), float(object[4]) - float(object[2]), edgecolor='r', facecolor='none'))

plt.show()
