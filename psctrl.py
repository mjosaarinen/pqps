#!/usr/bin/env python3

#	psctrl.py
#	2019-09-25	Markku-Juhani O. Saarinen <mjos@pqshield.com>
#
#	PowerShield control script

import sys
import time
import serial

#	PowerShield serial interface
pwsh = serial.Serial("/dev/serial/by-id/usb-STMicroelectronics_PowerShield__Virtual_ComPort_in_FS_Mode__FFFFFFFEFFFF-if00", 
	baudrate=3686400,
	bytesize=serial.EIGHTBITS,
	stopbits=serial.STOPBITS_ONE,
	parity=serial.PARITY_NONE,
	timeout=1)

#	Nucleo (target) serial interface
targ = serial.Serial("/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_0672FF535155878281153855-if02", 
	baudrate=115200,
	bytesize=serial.EIGHTBITS,
	stopbits=serial.STOPBITS_ONE,
	parity=serial.PARITY_NONE,
	timeout=1)

#	commands

def	pwsh_cmd(cmd):
	print("[CMD]", cmd, flush=True)
	pwsh.write((cmd + "\n").encode('ascii'))

def	targ_cmd(cmd):
	targ.write((cmd + "\n").encode('ascii'))

#	get a reply

def pwsh_get():
	s = ""	
	while True:
		x = pwsh.read()
		if (len(x) < 1):
			return s.replace("\0", "").strip().replace("\r", "").replace("\n\n\n", "\n")
		s += str(x, encoding='ascii', errors='ignore')

#	read target

def targ_get():
	s = ""
	while True:
		x = targ.read()
		if (len(x) < 1):
			return s.replace("\0", "").strip()
		s += str(x, encoding='ascii', errors='ignore')

#	until certain substring found

def	targ_get_str(st):
	print("[TARGET]")
	r = ""
	while r.find(st) == -1:
		s = targ_get()
		if s != "":
			print(s, flush=True)
		r += s
	return r

#	timeout in 1 sec

def	targ_resp():
	print("[TARGET]")
	print(targ_get(), flush=True)

#	until substring found

def	pwsh_get_str(st):
	r = ""
	while r.find(st) == -1:
		s = pwsh_get()
		if s != "":
			print(s, flush=True)
		r += s
	return r

#	"synchronize" powershield to get ok status

def pwsh_get_ok():
	r = ""
	while True:
		pwsh_cmd("status");
		s = pwsh_get()
		if s != "":
			print(s, flush=True)
		r += s
		if r.find("status ok") != -1:
			return r;
		time.sleep(1)


#	status

targ_resp()
pwsh_get_ok()

pwsh_cmd("htc")
pwsh_cmd("stop")
pwsh_cmd("trigsrc sw")
pwsh_cmd("temp degc")
pwsh_cmd("volt get")
pwsh_cmd("targrst 1")

pwsh_cmd("lcd 1 \"PowerShield PQC!\"")
pwsh_cmd("lcd 2 \"(c)2019 PQShield\"")
pwsh_get_ok()

#	configure the acquisiton
pwsh_cmd("output energy")
pwsh_cmd("format ascii_dec")
pwsh_cmd("freq 10")
pwsh_cmd("acqtime 10")
pwsh_cmd("acqmode dyn")

while pwsh_get().find("ack trigsrc d7") == -1:
	time.sleep(1)
	pwsh_cmd("trigsrc d7")

pwsh_cmd("start");
pwsh_get_ok()

#	there was a reset; get info
alg = targ_get_str("[INPUT]")

#	try to figure out if it's a kem or signature

kem = alg.find("CRYPTO_CIPHERTEXTBYTES") != -1

# 	get the algorithm name
i = alg.find("CRYPTO_ALGNAME")
if (i == -1):
	alg = "Unidentified";
else:
	alg = (alg[i:].split())[1]

pwsh_cmd("lcd 1 \""+alg[0:15]+"\"")
print("----", alg, "----", flush=True)

pwsh_cmd("lcd 2 \"Measuring All   \"")
print("[GET ALL]")
targ_cmd("a");
pwsh_get_str("Acquisition completed")
targ_get_str("[END]")

pwsh_cmd("lcd 2 \"Measuring KeyGen\"")
print("[KEYGEN]", flush=True)
targ_cmd("k");
pwsh_get_str("Acquisition completed")
targ_get_str("[END]")

if kem:

	pwsh_cmd("lcd 2 \"Measuring Encaps\"")
	print("[ENCAPS]", flush=True)
	targ_cmd("e");
	pwsh_get_str("Acquisition completed")
	targ_get_str("[END]")

	pwsh_cmd("lcd 2 \"Measuring Decaps\"")
	print("[DECAPS]", flush=True)
	targ_cmd("d");
	pwsh_get_str("Acquisition completed")
	targ_get_str("[END]")

else:

	pwsh_cmd("lcd 2 \"Measuring Sign  \"")
	print("[SIGN]", flush=True)
	targ_cmd("s");
	pwsh_get_str("Acquisition completed")
	targ_get_str("[END]")

	pwsh_cmd("lcd 2 \"Measuring Verify\"")
	print("[VERIFY]", flush=True)
	targ_cmd("v");
	pwsh_get_str("Acquisition completed")
	targ_get_str("[END]")

# baseline

pwsh_cmd("lcd 2 \"Baseline power  \"")
print("[BASELINE]")
targ_cmd("z");
pwsh_get_str("Acquisition completed")
targ_get_str("[END]")

# test it
pwsh_cmd("lcd 2 \"Diagnostic test \"")
print("[TESTING]")
targ_cmd("t");
targ_get_str("[END]")

pwsh_cmd("trigsrc sw")
pwsh_cmd("lcd 1 \"PowerShield PQC!\"")
pwsh_cmd("lcd 2 \"(c)2019 PQShield\"")
pwsh_get_ok()

sys.exit()

