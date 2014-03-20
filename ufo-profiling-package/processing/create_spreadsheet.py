#!/usr/bin/python

import sys
import re
import string
import textwrap
import argparse
import os
import csv
import itertools

methods = {
    'UFO(FBP)':['bp','fbp'],
    'UFO(DFI:Sinc,OS=1)':['dfi_os1','dfi-os1','os1-dfi'],
    'UFO(DFI:Sinc,OS=2)':['dfi_os2','dfi-os2','os2-dfi']
}

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
  parser = argparse.ArgumentParser(description='Spreadsheets generator')
  parser.add_argument('-i', '--input_path', required=True, type=str, help='the input path')
  parser.add_argument('-o', '--output_file_path', required=True, type=str, help='path to the file of output data')

  try:
    args = parser.parse_args()
  except argparse.ArgumentTypeError as err:
    print err
    sys.exit(1)

  devices = [f for f in os.listdir(args.input_path) if not f.startswith('.')]

  labels = []
  methods_results = {}

  for d in devices:
    labels.append(d.replace('_',' '))

    cur_path = os.path.join(args.input_path, d)
    methods_titles = os.listdir(cur_path)

    for method_title in methods_titles:
      path_method_value = os.path.join(cur_path, method_title)
      method_title = find_name(method_title)

      if not method_title in methods_results:
        methods_results[method_title] = []

      methods_results[method_title].insert(devices.index(d), get_method_value(path_method_value))

  print labels
  for key,value in methods_results.items():
    print key + ': ' + str(value)

  with open(args.output_file_path, 'wb') as csv_file:
    csv_writer = csv.writer(csv_file, delimiter=';', quotechar='|', quoting=csv.QUOTE_MINIMAL)
    
    csv_writer.writerow([""] + labels)
    for method_title,values in methods_results.items():
      csv_writer.writerow([method_title] + values)
  
  print "Spreadsheet has been created."
