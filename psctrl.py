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
	pwsh.write((cmd + "\n").encode('ascii'))

def	targ_cmd(cmd):
	targ.write((cmd + "\n").encode('ascii'))

#	get a reply

def pwsh_get():
	s = ""	
	while True:
		x = pwsh.read()
		if (len(x) < 1):
			return s.strip().replace("\r", "").replace("\n\n\n", "\n")
		s += str(x, encoding='ascii', errors='ignore')

#	read target

def targ_get():
	s = ""	
	while True:
		x = targ.read()
		if (len(x) < 1):
			return s.strip()
		s += str(x, encoding='ascii', errors='ignore')

def	targ_resp():
	print("[TARGET]")
	print(targ_get(), flush=True)

#	"synchronize" powershield to get ok status

def	pwsh_get_ok():
	while True:
		pwsh_cmd("status");
		ok = pwsh_get()
		print(ok, flush=True)
		if ok.find("ack status ok") != -1:
			return
		time.sleep(1)

def	pwsh_get_acq():
	acq = ""
	while acq.find("Acquisition completed") == -1:
		s = pwsh_get()
		print(s, flush=True)
		acq += s
	return acq

#	status
print("[TARGET]", targ_get())
pwsh_get_ok()

pwsh_cmd("temp degc")
pwsh_cmd("htc")
pwsh_cmd("stop")
pwsh_cmd("targrst 1")

pwsh_cmd("lcd 1 \"PowerShield PQC!\"")
pwsh_cmd("lcd 2 \"(c)2019 PQShield\"")
pwsh_get_ok()

#	configure the acquisiton
pwsh_cmd("output energy")
pwsh_cmd("format ascii_dec")
pwsh_cmd("freq 10")
pwsh_cmd("acqtime 5")
pwsh_cmd("acqmode dyn")
pwsh_cmd("trigsrc d7")
pwsh_cmd("start");
pwsh_get_ok()

#	there was a reset; get 
alg = targ_get()

#	try to figure out if it's a kem or signature

kem = alg.find("CRYPTO_CIPHERTEXTBYTES") != -1

# 	get the algorithm name
i = alg.find("CRYPTO_ALGNAME")
if (i == -1):
	alg = "Unidentified";
else:
	alg = (alg[i:].split())[1]
alg = alg[0:15]

pwsh_cmd("lcd 1 \""+alg+"\"")

pwsh_cmd("lcd 2 \"Baseline power  \"")
print("[BASELINE]")
targ_cmd("z");
pwsh_get_acq()
targ_resp()

pwsh_cmd("lcd 2 \"Measuring All   \"")
print("[GET ALL]")
targ_cmd("a");
pwsh_get_acq()
targ_resp()

#	change timing
pwsh_cmd("acqtime 1")
pwsh_get_ok()
targ_resp()

pwsh_cmd("lcd 2 \"Measuring KeyGen\"")
print("[KEYGEN]", flush=True)
targ_cmd("k");
pwsh_get_acq()
targ_resp()

if kem:

	pwsh_cmd("lcd 2 \"Measuring Encaps\"")
	print("[ENCAPS]", flush=True)
	targ_cmd("e");
	pwsh_get_acq()
	targ_resp()

	pwsh_cmd("lcd 2 \"Measuring Decaps\"")
	print("[DECAPS]", flush=True)
	targ_cmd("d");
	pwsh_get_acq()
	targ_resp()

else:

	pwsh_cmd("lcd 2 \"Measuring Sign  \"")
	print("[SIGN]", flush=True)
	targ_cmd("s");
	pwsh_get_acq()
	targ_resp()

	pwsh_cmd("lcd 2 \"Measuring Verify\"")
	print("[VERIFY]", flush=True)
	targ_cmd("v");
	pwsh_get_acq()
	targ_resp()

pwsh_cmd("lcd 1 \"PowerShield PQC!\"")
pwsh_cmd("lcd 2 \"(c)2019 PQShield\"")
pwsh_get_ok()

sys.exit()

