#!/usr/bin/env python3

#	parselog.py
#	2019-09-30	Markku-Juhani O. Saarinen <mjos@pqshield.com>

import sys
import time

systemcoreclock = 96000000.0
crypto_algname = ""
crypto_secretkeybytes = 0
crypto_publickeybytes = 0
crypto_ciphertextbytes = 0
crypto_bytes = 0

keygen = 0.0
encaps = 0.0
decaps = 0.0

lns = sys.stdin.readlines()
i = 0
watt = []
for lin in lns:
	lv = lin.split();
	ll = len(lv)

	#	basic data
	if ll == 2 and lv[0] == "SystemCoreClock":
		systemcoreclock=float(lv[1])
	if ll == 2 and lv[0] == "CRYPTO_ALGNAME":
		crypto_algname=lv[1]
	if ll == 2 and lv[0] == "CRYPTO_SECRETKEYBYTES":
		crypto_secretkeybytes=int(lv[1])
	if ll == 2 and lv[0] == "CRYPTO_PUBLICKEYBYTES":
		crypto_publickeybytes=int(lv[1])
	if ll == 2 and lv[0] == "CRYPTO_CIPHERTEXTBYTES":
		crypto_ciphertextbytes=int(lv[1])
	if ll == 2 and lv[0] == "CRYPTO_BYTES":
		crypto_bytes=int(lv[1])

	if ll > 1 and lv[0] == "TimeStamp:":
		v = []
		for x in lns[i+1:i+101]:
			v.append(float(x.replace("-","e-")))
		watt.append(10.0 * sum(v)/len(v))

	if ll >= 3 and lv[0] == "***":
		print(lin)
		if lv[1] == "KeyGen":
			keygen=float(lv[2])
		if lv[1] == "Encaps":
			encaps=float(lv[2]) 
		if lv[1] == "Decaps":
			decaps=float(lv[2])

	i = i + 1

print(crypto_algname)
print(crypto_secretkeybytes)
print(crypto_publickeybytes)
print(crypto_ciphertextbytes)
print(crypto_bytes)

print(f"KeyGen {keygen:10.0f} {1000*keygen/systemcoreclock:10.2f}ms {1000*watt[1]:10.2f}")

