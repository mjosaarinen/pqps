#!/usr/bin/env python3

#	parselog.py
#	2019-09-30	Markku-Juhani O. Saarinen <mjos@pqshield.com>

import sys
import time

core_clk = 96000000.0
alg_name = ""
sk_l = 0
pk_l = 0
ct_l = 0
data_l = 0

keygen 	= 0.0
encaps 	= 0.0
decaps 	= 0.0
sign	= 0.0
verify	= 0.0

samp_l = 0

lns = sys.stdin.readlines()
i = 0
watt = []
samp = []
sampling = False

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
		return f'{x:7.3f} '
	if (abs(x) < 1E6):
		return f'{1E-3*x:7.3f} k'
	if (abs(x) < 1E9):
		return f'{1E-6*x:7.3f} M'
	if (abs(x) < 1E12):
		return f'{1E-9*x:7.3f} G'
	return f'{x:8g}'

for lin in lns:
	lv = lin.split();
	ll = len(lv)

	#	basic data
	if ll == 2 and lv[0] == "SystemCoreClock":
		core_clk=float(lv[1])
	if ll == 2 and lv[0] == "CRYPTO_ALGNAME":
		alg_name=lv[1]
	if ll == 2 and lv[0] == "CRYPTO_SECRETKEYBYTES":
		sk_l = int(lv[1])
	if ll == 2 and lv[0] == "CRYPTO_PUBLICKEYBYTES":
		pk_l = int(lv[1])
	if ll == 2 and lv[0] == "CRYPTO_CIPHERTEXTBYTES":
		ct_l = int(lv[1])
	if ll == 2 and lv[0] == "CRYPTO_BYTES":
		data_l = int(lv[1])


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
			print(f"{i:d}: mismatch. length={len(samp):d} ", lin)

	if ll >= 3 and lv[0] == "***":
#		print(lin)
		if lv[1] == "KeyGen":
			keygen=float(lv[2])
		if lv[1] == "Encaps":
			encaps=float(lv[2]) 
		if lv[1] == "Decaps":
			decaps=float(lv[2])
		if lv[1] == "Sign":
			sign=float(lv[2]) 
		if lv[1] == "Verify":
			verify=float(lv[2])

	#	loop
	i = i + 1

kem = ct_l > 0;

if len(watt) < 4:
	print("Not enough data")
	exit(1)

def stats(cyc,pw):
	return units(cyc)+"cycles" + "\t" + units(cyc/core_clk)+"s" + "\t" + \
			units(pw)+"W" + "\t" + units(cyc*pw/core_clk)+"J"

if kem:
	print(f"{alg_name} (sk={sk_l}, pk={pk_l}, ct={ct_l}, ss={data_l})")
	print("KeyGen:", stats(keygen,watt[1]))
	print("Encaps:", stats(encaps,watt[2]))
	print("Decaps:", stats(decaps,watt[3]))
	print("TOTAL: ", stats(keygen+encaps+decaps,watt[0]))
else:
	print(f"{alg_name} (sk={sk_l}, pk={pk_l}, sl={data_l})")
	print("KeyGen:", stats(keygen,watt[1]))
	print("Sign:  ", stats(sign,watt[2]))
	print("Verify:", stats(verify,watt[3]))
	print("TOTAL: ", stats(keygen+sign+verify,watt[0]))

