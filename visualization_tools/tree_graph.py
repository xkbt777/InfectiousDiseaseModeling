import matplotlib.pyplot as plt
import sys
from matplotlib.patches import Rectangle

levels = {}
with open("tree", "r") as file:
    matrix_size = int(file.readline())
    for line in file:
        words = line.rstrip().split(",")
        print(words)
        if words[0] not in levels:
            levels[words[0]] = []
        levels[words[0]].append([float(words[1]), float(words[2]), float(words[3]), float(words[4])])

plt.axis([0, matrix_size, 0, matrix_size])
ax = plt.gca()

for words in levels[sys.argv[1]]:
    print(words)
    ax.add_patch(Rectangle((words[0], words[1]), words[2] - words[0], words[3] - words[1], edgecolor='r', facecolor='none'))

plt.show()