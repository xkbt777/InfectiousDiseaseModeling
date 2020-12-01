import matplotlib.pyplot as plt
import matplotlib
import numpy as np

label_list = ['Small', 'Medium', 'Large']
linear_scan = np.log([1 / 0.226, 100 / 24.841, 10000 / 1586.605])
r_tree = np.log([1 / 0.265, 100 / 13.978, 10000 / 419.629])
mpi_linear_scan = np.log([1 / 0.019, 100 / 0.771, 10000 / 34.342])
mpi_r_tree = np.log([1 / 0.023, 100 / 0.546, 10000 / 6.444])
x = range(len(linear_scan))

rects1 = plt.bar(left=x, height=linear_scan, width=0.2, alpha=0.8, color='red', label="linear_scan")
rects2 = plt.bar(left=[i + 0.2 for i in x], height=r_tree, width=0.2, color='green', label="r_tree")
rects3 = plt.bar(left=[i + 0.4 for i in x], height=mpi_linear_scan, width=0.2, alpha=0.8, color='black', label="mpi_linear_scan")
rects4 = plt.bar(left=[i + 0.6 for i in x], height=mpi_r_tree, width=0.2, alpha=0.8, color='blue', label="mpi_r_tree")
plt.ylim(0, 10)
plt.ylabel("log(Millions per second)")

plt.xticks([index + 0.2 for index in x], label_list)
plt.xlabel("Test size")
plt.title("Performance Graph")
plt.legend()

plt.show()