#!/sbin/env python

import re
import sys
from collections import defaultdict
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot as plt

with open(sys.argv[1], 'rb') as fhdl:
    raw_data = fhdl.readlines()

data = defaultdict(list)

for line in raw_data:
    key, value = line.decode('utf-8', 'replace').split(":", maxsplit=1)
    key = str(key.strip())
    if key not in ("Accel", "Gyro", "Magneto"):
        print("Could not process line: ", line)
        continue
    data[key].append([float(re.sub("[^0-9-.]", "", x.strip())) for x in value.split(',')])

print(data)

for key, value in data.items():
    for axis in zip(*value):
        print(axis)
        plt.plot(axis)
    plt.savefig('{}.png'.format(key))

