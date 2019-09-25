#!/usr/bin/env python3

#	psctrl.py
#	2019-09-25	Markku-Juhani O. Saarinen <mjos@pqshield.com>
#
#	PowerShield control script

import sys
import time
import serial

#	PowerShield serial interface
ser_ps = serial.Serial("/dev/serial/by-id/usb-STMicroelectronics_PowerShield__Virtual_ComPort_in_FS_Mode__FFFFFFFEFFFF-if00", 
	baudrate=3686400,
	bytesize=serial.EIGHTBITS,
	stopbits=serial.STOPBITS_ONE,
	parity=serial.PARITY_NONE,
	timeout=1)

#	Nucleo (target) serial interface
ser_nu = serial.Serial("/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_0672FF535155878281153855-if02", 
	baudrate=115200,
	bytesize=serial.EIGHTBITS,
	stopbits=serial.STOPBITS_ONE,
	parity=serial.PARITY_NONE,
	timeout=1)

#	command

def	ps_command(cmd):
	ser_ps.write(("\n" + cmd + "\n").encode('ascii'))

#	get a reply

def ps_get_reply():
	s = ""	
	while True:
		x = ser_ps.read()
		if (len(x) < 1):
			return s.strip()
		s += str(x, encoding='ascii', errors='ignore')

#	"synchronize" powershield to get ok status

def	ps_get_ok():

	while True:
		ps_command("status");
		ok = ps_get_reply()
		print("[PS]", ok)
		if ok.find("ack status ok") != -1:
			return
		time.sleep(1)

#	status
ps_command("stop")
ps_command("targrst 1")
ps_command("temp degc")
ps_get_ok()
ps_command("htc")
ps_command("lcd 1 \"PQPS Measurement\"")
ps_command("lcd 2 \"(c) PQShield Ltd\"")
ps_get_ok()

time.sleep(1)

#	configure the acquisiton
ps_command("output energy")
ps_command("freq 5")
ps_command("acqtime 5")
ps_command("acqmode dyn")
ps_command("start");
ps_command("trigsrc d7")

ps_get_ok()
while True:
	print("[PS]", ps_get_reply())

