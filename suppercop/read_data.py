#!/usr/bin/env python3

#	read_data.py
#	2019-09-30	Markku-Juhani O. Saarinen <mjos@pqshield.com>

#	simple (stdin) parser for supercop data

import sys
import time

lnum = 0
vuj = []
vac = []
for line in sys.stdin:
	lnum = lnum + 1
	v = line.split()
	if len(v) < 9:
		continue
	if v[6][-7:] == "_ujoule":
		vuj = v
	if v[6][-7:] == "_avgcyc":
		vac = v
		# avgcyc comes usually right after ujoule
		if vuj[0:5] == vac[0:5] and vuj[7] == vac[7] and vuj[6][:-7] == vac[6][:-7]:
			alg = vuj[5]
			mes = vuj[6][:-7]
			uj = float(vuj[8])
			cyc = float(vac[8])
			njc = 1E3*uj/cyc
			# name, measurement, microjoules, cycles, nanojoules/cycle
			print(f"{alg:24s}{mes:8s}{uj:8.0f} uJ {cyc:12.0f} cc {njc:12.4f} nJ/cc")
