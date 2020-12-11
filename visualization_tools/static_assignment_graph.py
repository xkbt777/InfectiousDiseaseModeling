import matplotlib.pyplot as plt
import sys
from matplotlib.patches import Rectangle

matrix_size = 256
p_num = 8
radius = 4

plt.axis([0, matrix_size, 0, matrix_size])
ax = plt.gca()

for i in range(8):
    bottom = i * matrix_size / p_num
    left = 0
    ax.add_patch(Rectangle((left, bottom), matrix_size, matrix_size / p_num, edgecolor='r', facecolor='none'))
    cx = matrix_size / 2.0
    cy = bottom + matrix_size / (2 * p_num)
    ax.annotate(i, (cx, cy), color='r', weight='bold', fontsize=6, ha='center', va='center')

for i in range(7):
    bottom = ((i + 1) * matrix_size / p_num) - radius
    left = 0
    ax.add_patch(Rectangle((left, bottom), matrix_size, 2 * radius, edgecolor='g', facecolor='none'))

ax.add_patch(Rectangle((0, 0), 0,0, edgecolor='r', facecolor='none', label='assign area'))
ax.add_patch(Rectangle((0, 0), 0,0, edgecolor='g', facecolor='none', label='ghost zone'))

plt.legend()
plt.show()