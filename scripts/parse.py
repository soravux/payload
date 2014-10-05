#!/sbin/env python

import re
import sys
from collections import defaultdict
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot as plt


def parseFile(filename):
    with open(filename, 'rb') as fhdl:
        raw_data = fhdl.readlines()

    data = defaultdict(list)

    for line in raw_data:
        key, value = line.decode('utf-8', 'replace').split(":", maxsplit=1)
        key = str(key.strip())
        if key not in ("Accel", "Gyro", "Magneto"):
            print("Could not process line: ", line)
            continue
        data[key].append([float(re.sub("[^0-9-.]", "", x.strip())) for x in value.split(',')])
    return data


def plotData(data):
    for key, value in data.items():
        print(key)
        for axis in zip(*value):
            print(axis)
            plt.plot(axis)
        plt.savefig('{}.png'.format(key))


if __name__ == '__main__':
    data = parseFile(sys.argv[1])
    plotData(data)
