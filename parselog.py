#!/usr/bin/env python3

#	parselog.py
#	2019-09-30	Markku-Juhani O. Saarinen <mjos@pqshield.com>

#	parse a single measurement run (from stdin)

import sys
import time



# prettyprint units

def units(x):
	if (abs(x) < 1E-12):
		return f'{x:10g}'
	if (abs(x) < 1E-9):
		return f'{1E12*x:7.3f} p'
	if (abs(x) < 1E-6):
		return f'{1E9*x:7.3f} n'
	if (abs(x) < 1E-3):
		return f'{1E6*x:7.3f} μ'
	if (abs(x) < 1.0):
		return f'{1E3*x:7.3f} m'
	if (abs(x) < 1E3):
		return f'{x:7.3f}  '
	if (abs(x) < 1E6):
		return f'{1E-3*x:7.3f} k'
	if (abs(x) < 1E9):
		return f'{1E-6*x:7.3f} M'
	if (abs(x) < 1E12):
		return f'{1E-9*x:7.3f} G'
	return f'{x:8g}'

def stats(cyc,pw):
	return units(cyc)+"clk" + "\t" + units(cyc/cclk)+"s" + "\t" + \
			units(pw)+"W" + "\t" + units(cyc*pw/cclk)+"J"

# the main data dictionaries

kems = {}
sgns = {}

# parse input files

for fn in sys.argv[1:]:
	
	with open(fn, 'r') as f:
		lns = f.readlines()

	watt = []
	samp = []
	sampling = False

	cclk = 96000000.0
	algn = ""
	sk_l = 0
	pk_l = 0
	ct_l = 0
	ss_l = 0

	keygen 	= 0.0
	encaps 	= 0.0
	decaps 	= 0.0
	sign	= 0.0
	verify	= 0.0

	i = 0
	for lin in lns:

		#	line number
		i = i + 1
		lv = lin.split();
		ll = len(lv)

		#	basic parameters
		if ll == 2 and lv[0] == "SystemCoreClock":
			cclk = float(lv[1])
		if ll == 2 and lv[0] == "CRYPTO_ALGNAME":
			algn = lv[1]
		if ll == 2 and lv[0] == "CRYPTO_SECRETKEYBYTES":
			sk_l = int(lv[1])
		if ll == 2 and lv[0] == "CRYPTO_PUBLICKEYBYTES":
			pk_l = int(lv[1])
		if ll == 2 and lv[0] == "CRYPTO_CIPHERTEXTBYTES":
			ct_l = int(lv[1])
		if ll == 2 and lv[0] == "CRYPTO_BYTES":
			ss_l = int(lv[1])

		# process samples
		if ll > 1 and lv[0] == "TimeStamp:":
			samp = []
			sampling = True

		if ll == 1 and lv[0] == "end":
			sampling = False

		if ll == 1 and sampling:
			samp.append(float(lv[0].replace("-","e-")))

		if ll >= 3 and lv[0] == "Acquisition" and lv[1] == "time:":
			acq_time = float(lv[2])
			watt.append(sum(samp)/acq_time)

		if ll >= 4 and lv[0] == "Number" and lv[2] == "samples:":
			if int(lv[3]) != len(samp):
				print(f"{fn}:{i}[have {len(samp)}]{lin}", file=sys.stderr)

		if ll >= 3 and lv[0] == "***":
			if lv[1] == "KeyGen":
				keygen = float(lv[2])
			if lv[1] == "Encaps":
				encaps = float(lv[2]) 
			if lv[1] == "Decaps":
				decaps = float(lv[2])
			if lv[1] == "Sign":
				sign = float(lv[2]) 
			if lv[1] == "Verify":
				verify = float(lv[2])

	if len(watt) < 4:
		print(f"{fn}: not enough data.", file=sys.stderr)
		continue

	kem = ct_l > 0

	if fn.find("sneik") != -1:
		algn = algn + "-sneik"		# add identifier (otherwise same)

	#	only cca
#	if algn.find("Ephem") != -1:
#		continue
#	if algn.find("CPAKEM") != -1:
#		continue

	#	take the best implementation

	if kem:
		tot = keygen + encaps + decaps
		if algn in kems and kems[algn][0][0] < tot:
			continue
		kems[algn] = ( (tot, keygen, encaps, decaps), tuple(watt), 
						(sk_l, pk_l, ct_l, ss_l) )
	else:
		tot = keygen + sign + verify
		if algn in sgns and sgns[algn][0][0] < tot:
			continue
		sgns[algn] = ( (tot, keygen, sign, verify), tuple(watt),
						(sk_l, pk_l, ss_l) )

#	really short summaries
#for x in sgns:
#	s = f"| {x:20} |"
#	for i in [ 1, 2, 3, 0 ]:
#		s += " " + units(sgns[x][0][i] * sgns[x][1][i] / cclk) + "J |"
#	print(s)
#for x in kems:
#	s = f"| {x:20} |"
#	for i in [ 1, 2, 3, 0 ]:
#		s += " " + units(kems[x][0][i] * kems[x][1][i] / cclk) + "J |"
#	print(s)

#	summary results

for x in kems:
	print(f"{x} (sk={kems[x][2][0]}, pk={kems[x][2][1]}, " +
			f"ct={kems[x][2][2]}, ss={kems[x][2][3]})")
	print("KeyGen:", stats(kems[x][0][1], kems[x][1][1]))
	print("Encaps:", stats(kems[x][0][2], kems[x][1][2]))
	print("Decaps:", stats(kems[x][0][3], kems[x][1][3]))
	print("TOTAL: ", stats(kems[x][0][0], kems[x][1][0]))
	print()

for x in sgns:
	print(f"{x} (sk={sgns[x][2][0]}, pk={sgns[x][2][1]}, " + 
			f"sl={sgns[x][2][2]})")
	print("KeyGen:", stats(sgns[x][0][1], sgns[x][1][1]))
	print("Sign:  ", stats(sgns[x][0][2], sgns[x][1][2]))
	print("Verify:", stats(sgns[x][0][3], sgns[x][1][3]))
	print("TOTAL: ", stats(sgns[x][0][0], sgns[x][1][0]))
	print()

