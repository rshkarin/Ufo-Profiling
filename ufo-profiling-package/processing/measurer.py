#!/usr/bin/python

import sys
import re
import string
import textwrap
import argparse
import math

measure = "ms"

if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='Measurer')
	parser.add_argument('-i', '--input_file_path', required=True, type=str, help='path to the file of input data')
	parser.add_argument('-o', '--output_file_path', required=True, type=str, help='path to the file of output data')
	parser.add_argument('-d', '--device', required=False, type=str, help='device for measuring')
	parser.add_argument('-m', '--method', required=False, type=str, help='method of reconstruction')

	try:
		args = parser.parse_args()
	except argparse.ArgumentTypeError as err:
		print err
		sys.exit(1)

	try:
		fo = open(args.input_file_path, 'r')
	except IOError as err:
		print "The file {0} doesn't exist!".format(args.input_file_path)
		sys.exit(1)

	data = fo.readlines()
	fo.close()

	profilings = []
	i = 0
	for line in data:
	  larr = line.split(' ')
	  profilings.append((larr[0], (float(larr[5]) - float(larr[4]))/1000000))
	  
	result = {}
	temp_fft_values = []
	temp_fft_keys = []
	new_fft_key = ''
	new_fft_value = 0

	for item in profilings:
	  key = item[0]
	  value = item[1]

	  if key.find('fft') != -1:
		if key in temp_fft_keys:
		  new_fft_key = 'fft' + str(len(temp_fft_values))
		  new_fft_value = sum(temp_fft_values)

		  if new_fft_key in result:
			if result[new_fft_key] >= new_fft_value:
			  result[new_fft_key] = new_fft_value
		  else:
			result[new_fft_key] = new_fft_value

		  temp_fft_values = []
		  temp_fft_keys = []

		temp_fft_values.append(value)
		temp_fft_keys.append(key)
	  else:
	  	if len(temp_fft_values) and len(temp_fft_keys):
	  	  new_fft_value = sum(temp_fft_values)

		  if new_fft_key in result:
			if result[new_fft_key] >= new_fft_value:
			  result[new_fft_key] = new_fft_value
		  else:
			result[new_fft_key] = new_fft_value

		  temp_fft_values = []
		  temp_fft_keys = []
		  new_fft_key = ''
		  new_fft_value = 0

		if key in result:
		  if result[key] >= value:
			result[key] = value
	  	else:
		  result[key] = value
	
	result['total_time'] = sum(result.values())

	if args.device:
	  result['device'] = args.device.replace(' ', '_')

	if args.method:
	  result['method'] = args.method.replace(' ', '_')

	fo = open(args.output_file_path, 'w')

	for key, val in result.iteritems() :
	  fo.write(key + ' ' + str(val) + '\n')

	fo.close()
	print 'Output data has been written to ' + args.output_file_path

	

	
	  
