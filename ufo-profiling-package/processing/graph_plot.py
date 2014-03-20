#!/usr/bin/python

import sys
import re
import string
import textwrap
import argparse
import math
import os
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.cm as cmx
import numpy as np

plt.ioff()

methods = {
    'UFO(FBP)':['bp','fbp'],
    'UFO(DFI:Sinc,OS=1)':['dfi_os1','dfi-os1','os1-dfi'],
    'UFO(DFI:Sinc,OS=2)':['dfi_os2','dfi-os2','os2-dfi']
}

colors = [
    (200./255.,55./255.,55./255.),
    (255./255.,204./255.,0./255.),
    (85./255.,212./255.,0./255.),
    (141./255.,95./255.,211./255.)
]

def find_name(title):
    for key in methods.keys():
        variants = methods[key]
        for variant in variants:
            if (variant in title) or (title in variant):
                return key

def get_method_value(file_path):
    try:
        fo = open(file_path, 'r')
    except IOError as err:
        print "The file {0} doesn't exist!".format(file_path)
        sys.exit(1)

    data = fo.readlines()
    fo.close()

    for line in data:
        values = line.split(' ')
        if values[0] == 'total_time':
            return float(values[1])


if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='Graphs creator')
  parser.add_argument('-i', '--input_path', required=True, type=str, help='the input path')
  parser.add_argument('-o', '--output_file_path', required=True, type=str, help='path to the file of output data')
  parser.add_argument('-b', '--bar_width', required=False, type=str, help='width of each bar on graph [0 - 1.0]')

  try:
    args = parser.parse_args()
  except argparse.ArgumentTypeError as err:
    print err
    sys.exit(1)

  devices = [f for f in os.listdir(args.input_path) if not f.startswith('.')]

  xticks_labels = []
  methods_results = {}

  for d in devices:
    xticks_labels.append(d.replace('_',' '))

    cur_path = os.path.join(args.input_path, d)
    methods_titles = os.listdir(cur_path)

    for method_title in methods_titles:
      path_method_value = os.path.join(cur_path, method_title)
      method_title = find_name(method_title)

      if not method_title in methods_results:
        methods_results[method_title] = []

      methods_results[method_title].insert(devices.index(d), get_method_value(path_method_value))

  #for key,value in methods_results.items():
  #  print key + ': ' + str(value)

  N = len(xticks_labels)

  #the x locations for the groups
  ind = np.arange(N)    

  if args.bar_width:
    width = args.bar_width
  
  #the width of the bars: can also be len(x) sequence
  width = 0.2
  
  legend_labels = tuple(methods_results.keys())
  fig = plt.figure(figsize=(17,10),dpi=150)
  ax = fig.add_subplot(111)

  p = []
  x_coord = ind + width
  keys = methods_results.keys()
  max_value = 0

  
  for line in ax.get_yticklines():
    line.set_markersize(0)

  for val in methods_results.values():
    print val
    max_value = np.floor(max(val))
    max_value = max_value + np.floor(max_value/15)

  for i in range(0,len(keys)):
    p.append(ax.barh(x_coord, tuple(methods_results[keys[i]]), width, color=colors[i]))
    x_coord = x_coord + width

  plt.xlim((0, max_value))
  plt.xlabel('Time (ms)', size=18)
  plt.tick_params(\
    axis='y',          #changes apply to the x-axis
    which='both',      #both major and minor ticks are affected
    bottom='off',      #ticks along the bottom edge are off
    top='off',         #ticks along the top edge are off
    labelbottom='off', #labels along the bottom edge are off
    labelsize=18)

  plt.yticks(ind + 0.5, xticks_labels)
  plt.xticks(np.arange(0, max_value, 10))

  #get position box of legend
  box = ax.get_position()

  #shink current axis's height by 10% on the bottom
  ax.set_position([box.x0 + box.width*0.2, box.y0 + box.height*0.2, box.width*0.8, box.height*0.8])

  #put a legend below current axis
  ax.legend(tuple(p), 
            legend_labels, 
            loc='upper center', 
            bbox_to_anchor=(0.5, -0.1), 
            fancybox=True, 
            shadow=True, 
            ncol=2)

  fig.savefig(args.output_file_path)
