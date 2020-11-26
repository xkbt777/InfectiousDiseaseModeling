import matplotlib.pyplot as plt
import sys
from matplotlib.patches import Rectangle

objects = []
with open(sys.argv[1], "r") as file:
    matrix_size = int(file.readline())
    for line in file:
        objects.append(line.rstrip().split(","))

plt.axis([0, matrix_size, 0, matrix_size])

center_s_x = []
center_s_y = []

center_i_x = []
center_i_y = []

center_r_x = []
center_r_y = []

for object in objects:
    cx = (float(object[1]) + float(object[3])) / 2.0
    cy = (float(object[2]) + float(object[4])) / 2.0

    if int(object[5]) == 1:
        center_i_x.append(cx)
        center_i_y.append(cy)
    elif int(object[5]) == 2:
        center_r_x.append(cx)
        center_r_y.append(cy)
    else:
        center_s_x.append(cx)
        center_s_y.append(cy)

plt.scatter(center_s_x, center_s_y, 1, color='b')
plt.scatter(center_i_x, center_i_y, 1, color='r')
plt.scatter(center_r_x, center_r_y, 1, color='g')
plt.show()
