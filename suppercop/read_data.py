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
	if v[6][-7:] == "_njoule" or v[6][-7:] == "_avgcyc":
		if v[7] == "-":
			mes = v[6]
		else:
			mes = v[7]+"."+v[6]		
		if v[5] in data:
			if v[6] in data[v[5]]:
				data[v[5]][mes].append(float(v[8]))
			else:
				data[v[5]][mes] = [ float(v[8]) ];
		else:
			data[v[5]] = { mes : [ float(v[8]) ] }

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
		if x[0][-7:] == "_njoule":
			mes = x[0][:-7]				# name of measurement
			nj = mfavg(x[1], 0.75)		# get average joules
			# get related cycles
			cyc = mfavg(alg[1][mes+"_avgcyc"],0.75)
			njc = nj/cyc				# nanojoules per cycle

			# name, measurement
			mss=mes.split(".")
			if len(mss) == 2:
				y = int(int(mss[0]) / 1000000)
				x = int(int(mss[0]) % 1000000)
				mes = mss[1]
			else:
				x = 0
				y = 0

			print(f"{alg[0]:32s} {mes:10s} {x:5d} {y:5d} ", end='')

			# nanojoules, cycles, nanojoules/cycle
			print(f"{nj:12.0f} nJ {cyc:12.0f} clk {njc:8.4f} nJ/clk")

