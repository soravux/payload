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
        ts, key = (x.strip() for x in key.split("|", maxsplit=1))
        if key not in ("Accel", "Gyro", "Magneto"):
            print("Could not process line: ", line)
            continue
        try:
            data[key].append([float(re.sub("[^0-9-.]", "", x.strip())) for x in value.split(',')])
        except ValueError:
            print("Could not convert values to float on line: ", line)
    return data


def plotData(data):
    for key, value in data.items():
        for axis in zip(*value):
            plt.plot(axis)
        plt.savefig('{}.png'.format(key))
        plt.clf()


if __name__ == '__main__':
    data = parseFile(sys.argv[1])
    plotData(data)
