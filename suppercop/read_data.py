#!/usr/bin/env python3

#	read_data.py
#	2019-09-30	Markku-Juhani O. Saarinen <mjos@pqshield.com>

#	simple (stdin) parser for supercop data

import sys
import time
import math

#	read in the data lines we want

lnum = 0								# line number (debug)
data = {}								# double dictionary

for line in sys.stdin:

	lnum = lnum + 1
	v = line.split()
	if len(v) < 9:
		continue

	# we are only interested in these two types of data
	if v[6][-7:] == "_ujoule" or v[6][-7:] == "_avgcyc":
		
		if v[5] in data:
			if v[6] in data[v[5]]:
				data[v[5]][v[6]].append(float(v[8]))
			else:
				data[v[5]][v[6]] = [ float(v[8]) ];
		else:
			data[v[5]] = { v[6] : [ float(v[8]) ] }

#	"median filter average"

def mfavg(l, c):
	l.sort()
	md = l[int(len(l) / 2)]
	lo = md * c
	hi = md / c
	s = 0.0
	n = 0.0
	for x in l:
		if x >= lo and x <= hi:
			s += x
			n += 1
	return s / n

#	process and output data

for alg in data.items():
	for x in alg[1].items():
		if x[0][-7:] == "_ujoule":
			mes = x[0][:-7]				# name of measurement
			uj = mfavg(x[1], 0.75)		# get average joules
			# get related cycles
			cyc = mfavg(alg[1][mes+"_avgcyc"],0.75)
			njc = 1E3*uj/cyc			# nanojoules per cycle
			# name, measurement, microjoules, cycles, nanojoules/cycle
			print(f"{alg[0]:32s}{mes:8s}{uj:10.0f} uJ {cyc:11.0f} clk {njc:8.4f} nJ/clk")

