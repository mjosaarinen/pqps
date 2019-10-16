# pqps (Post-Quantum Power Sandwich)

2019-10-01  Markku-Juhani O. Saarinen <mjos@pqshield.com>

**Experimental** tools for PQC energy measurements.

# What's this about ?

My original motivation was to establish a straightforward model from 
"cycle counts" to "Joules" for new (post-quantum) asymmetric cryptographic 
algorithms. The null hypothesis was that the relationship is largely linear 
and algorithm-independent. This turned out *not* to be true for the Cortex M4 
microcontrollers; there is 50+% variation in average power depending on what
type of cryptographic primitive the MCU is processing. See bottom of this 
page for an initial summary.

I also "modded" the SUPERCOP Cryptographic Benchmark
to do similar energy measurements on desktop/laptop/server systems using 
Intel's built-in RAPL energy counters. See the directory 
[pqps/suppercop](suppercop) for a discussion about that.

##  Embedded Measurements

This little tutorial and software package explains how to use the
[X-NUCLEO-LPM01A](https://www.st.com/en/evaluation-tools/x-nucleo-lpm01a.html)
-- a high-precision controlled power supply board -- to measure power 
consumption of cryptographic algorithms of a Cortex M4 target. The board is 
commonly called "PowerShield" in ST documentation, and I'll use that name 
here as well. Note that PowerShield is also used in the "industry standard" 
Ultra-Low Power (ULP) benchmark [ULPMark](https://www.eembc.org/ulpmark/).

PowerShield can be used in many ways, but its main intended configuration
is to create a development board **sandwich** with a STM32 Nucleo-32/64/144 
board (the number refers to number of pins in the LQFP or UFQFPN packaging of 
the MCU chip) using Arduino connectors on both boards. So we have two 
boards on top of each other, PowerShield on top, supplying power to the 
target board:

![PowerShield Sandwich](assets/powershield.jpg)

This set-up excludes some older boards such as
[STM32F407G-DISC1](https://www.st.com/en/evaluation-tools/stm32f4discovery.html)
that is used by the [PQM4](https://github.com/mupq/pqm4) project where I will
be lifting most of the evaluation targets from. The "discovery board" is
also little bit awkward as it requires an additional USB serial 
dongle (or a hardware mod) for serial communications.

So I chose to use a little bit cheaper
[NUCLEO-F411RE](https://www.st.com/en/evaluation-tools/nucleo-f411re.html)
board. This board also supports external SMPS (switched-mode power supply)
for Vcore logic supply, which is precisely what we want and use. 
Mostly the specs of STM32F411RE (the chip on this Nucleo64) are similar to
STM32F407VGT6 (the chip on Discovery); importantly both are Cortex M4 with 
(single-precision) floating point and DSP instructions. The F411RE has
only 512kB of Flash and 128kB of SRAM and lacks a TRNG, and can be
clocked only to 100 MHz; however it comes from the "Dynamic Efficiency" 
product line, being hopefully more efficient than the Discovery.


**Manuals**

* [UM2243](https://www.st.com/resource/en/user_manual/dm00406577.pdf)
User Manual: STM32 Nucleo expansion board for power consumption measurement

* [UM2269](https://www.st.com/resource/en/user_manual/dm00418905.pdf)
User manual: Getting started with PowerShield firmware

* [UM1724](https://www.st.com/resource/en/user_manual/dm00105823.pdf)
User Manual: STM32 Nucleo-64 boards (MB1136) 

* [STM32F411RE](https://www.st.com/resource/en/datasheet/stm32f411re.pdf)
Datasheet: STM32F411xC STM32F411xE Arm Cortex-M4 32b MCU+FPU, 125 DMIPS, 
512KB Flash, 128KB RAM, USB OTG FS, 11 TIMs, 1 ADC, 13 comm. interfaces

* [RM0383](https://www.st.com/resource/en/reference_manual/dm00119316.pdf)
Reference Manual: STM32F411xC/E advanced Arm-based 32-bit MCUs (big!)


### So.. What kind of gear do I need?

I use [Ubuntu Linux](https://ubuntu.com/desktop) 18.04 LTS and only open 
source software tools for this lab.

We paid £53.68 for the X-NUCLEO-LPM01A (PowerShield) board and £10.48 for 
NUCLEO-F411RE (Target) with a local UK component retailer in September 2019.
There seems to be good availablity of these boards; we had them delivered
the following day.

The PowerShield has a micro-B USB connector, while Nucleo-64 boards have a
Mini-B connector. So you need both of these cables to connect to your PC's 
Type-A (that common big USB) or Type-C (new) connectors (£5). 

It's helpful to have an USB extension cable and an USB hub with individual
on/off switches for each ports (something like £10).

#### Software

For building on Debian/Ubuntu you'll need "make" and other build essentials 
(`sudo apt install build-essential`) and also a suitable ARM cross compiler.

You can install the default compilers with `sudo apt install gcc-arm-none-eabi`
(or similar). I'm using 
`GNU Tools for Arm Embedded Processors 8-2019-q3-update` downloaded
from the [ARM Website](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) myself.

For flashing the target I use `st-flash` which is part of the open source
[stlink](https://github.com/texane/stlink) package. I built this from 
sources.

The scripts use bash and Python3 for serial communication
(`sudo apt install python3-serial` and dependencies).


## Measurement Mods

UM1724 states that "SB12 NRST (target STM32 RESET) must be OFF if CN4 pin 5 is 
used in the external application." Anyway, this is not your typical 
accidental solder bridge but a surface-mounted zero-ohm resistor. 
Just carefully heat both ends of the resistor until it comes off. 
Since there is some solder residue, I used a multimeter to check that
the connection is really off.

There are other measurement options, but this is the currenct configuration
used in measurements.

* On PowerShield: Jumpers of power supply pin: Close jumper AREF_ARD, 
	open jumper 3V3_ARD.
* On Nucleo64: Open Jumper IDD. 
* On Nucleo64: Remove solder bridge SB12 to disconnect reset signal
from ST-Link part.
* On Nucleo64: Remove solder bridge SB2 disconnecting 3.3V voltage
	regulator LD39050PU33R.

I followed the instructions in Section 1.2 of UM2269 "Quick setup to measure 
current on board Nucleo64" (AREF_ARD, 3V3_ARD, IDD, SB12) and additionally
disconnected SB2; Section 6.3.3 of UM1724 "External power supply input: +3.3V".

| <img src="assets/pqps_jumpers.jpg"> | <img src="assets/idd_jumper.jpg"> | <img src="assets/sb12_solder.jpg"> | <img src="assets/sb2_solder.jpg"> |
|:---:|:---:|:---:|:---:|
|Jumpers on PowerShield.|IDD Jumper on Nucleo64.|Solder Bridge SB12.|Solder Bridge SB2.|

The current measurement program (`main.cpp`) uses pin D7 on the Nucleo64 board
to trigger beginning of measurements. This is supported by the standard 
firmware.

### Note on the serial interface

We will be dealing with two serial interfaces simultaneously; on my
system they appear as `/dev/ttyACM0` and  `/dev/ttyACM1`. In order to 
find their unique identifier paths, you can list and access them by
their unique identifiers: `ls /dev/serial/by-id`. 

The PowerShield line feed dicipline is little strange. For direct access
I use picocom (`sudo install picocom`) like this (as noted, the path
is little different for you due to the unique identifier):

```
picocom --echo --imap lfcrlf --omap crlf /dev/serial/by-id/usb-STMicroelectronics_PowerShield__Virtual_ComPort_in_FS_Mode__FFFFFFFEFFFF-if00
```

Try issuing the `help` command to get a command summary. 

UM2269 states that the PowerShield baudrate is 3686400, but I really don't 
know how to make that work (and it actually sounds littble bit crazy). 
`setserial -av <powershield device>` gives a base rate of 38400 anyway.

My pqps firmware is configured to communicate at 115200 baud; add `-b 115200`
as a picocom parameter if needed. This is easily changed in `main.cpp`.

### Compiling PQC implementations from PQM4

The measurement was ran on implementations from the 
[pqm4](https://github.com/mupq/pqm4) "Post-quantum crypto library for the 
ARM Cortex-M4". However I'm using a different target board and the 
[ARM Mbed OS v2](https://os.mbed.com/) runtime libraries instead of
[libopencm3](https://github.com/libopencm3) that the pqm4 uses. 

So, first get the distribution and unpack pqm4 inside it:
```
git clone https://github.com/mjosaarinen/pqps.git
cd pqps
git clone --recursive https://github.com/mupq/pqm4.git
```

If everything is fine, you can attempt to build some target with

```
$ make PQALG=pqm4/crypto_kem/kyber768/m4

[CPP] main.cpp
[..]
[AS] fastntt.S
cp ../mbed/TARGET_NUCLEO_F411RE/TOOLCHAIN_GCC_ARM/STM32F411XE.ld pqps.link_script.ld
link: pqps.elf
arm-none-eabi-objcopy -O ihex pqps.elf pqps.hex
```

The `PQALG` argument is needed and points to a directory containing
the target implementation. 

### Compiling ECDSA and ECDH from "micro-ecc"

The directory [mecc](mecc) contains wrappers that I wrote for Kem MacKay's 
[micro-ecc](https://github.com/kmackay/micro-ecc), which may be used 
as a reference point. The compilation and testing mechanism is exactly the
same as for PQM4 algorithms (ECDH is modeled as a KEM).
See the [mecc](mecc) README for more information.

### Flashing

If the build was successful, `BUILD/pqps.hex` 
contains a firmware image that can be flashed with stlink:

```
$ st-flash --format ihex write BUILD/pqps.hex

st-flash 1.5.1-16-g1165cf7
2019-10-01T13:36:18 INFO common.c: Loading device parameters....
2019-10-01T13:36:18 INFO common.c: Device connected is: F4 device (low power) - stm32f411re, id 0x10006431
2019-10-01T13:36:18 INFO common.c: SRAM size: 0x20000 bytes (128 KiB), Flash: 0x80000 bytes (512 KiB) in pages of 16384 bytes
2019-10-01T13:36:18 INFO common.c: Attempting to write 84332 (0x1496c) bytes to stm32 address: 134217728 (0x8000000)
Flash page at addr: 0x08010000 erasedEraseFlash - Sector:0x4 Size:0x10000 
2019-10-01T13:36:20 INFO common.c: Finished erasing 5 pages of 65536 (0x10000) bytes
2019-10-01T13:36:20 INFO common.c: Starting Flash write for F2/F4/L4
2019-10-01T13:36:20 INFO flash_loader.c: Successfully loaded flash loader in sram
Target voltage (51 mV) too low for 32-bit flash, using 8-bit flash writes
size: 32768
size: 32768
size: 18796
2019-10-01T13:36:22 INFO common.c: Starting verification of write complete
2019-10-01T13:36:23 INFO common.c: Flash written and verified! jolly good!
```

**NOTE.**  The target board needs power to be programmed; on initial
run you can allow this via the menus (and joystick) on the PowerShield;
just press the big blue "ENTER" button and check the LCD display.

You can try interacting with the default firmware with (replace the
device identifier with that of your target):

```
$ picocom --echo -b 115200 --imap lfcrlf --omap crlf /dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_0672FF535155878281153855-if02

[...]
Terminal ready

[RESET] This is PQPowerShield! Welcome.

SystemCoreClock         96000000
CRYPTO_ALGNAME          Kyber768
CRYPTO_SECRETKEYBYTES   2400
CRYPTO_PUBLICKEYBYTES   1184
CRYPTO_CIPHERTEXTBYTES  1088
CRYPTO_BYTES            32
[INPUT] a = all, k = keygen, e = encaps, d = decaps
a
[START] measure(10000,kg,enc,dec)
[END] 10023 milliseconds, n=298 (0 errors)
*** KeyGen       978424 Kyber768
*** Encaps      1150505 Kyber768
*** Decaps      1099639 Kyber768
*** Total       3228569 Kyber768
[INPUT] a = all, k = keygen, e = encaps, d = decaps
```

### Measurements

* Take a look at `main.cpp` to see what you want to actually measure.

* The script running a set of energy measurements is contained in `psctrl.py`;
you'll have to modify at least the serial device identifiers for this to work.

* A shell script `test_alg.sh` will measure every implementation it can find,
in random order and write results to `logs`.

* A python script `parselog.py` is used to interpret these results.

* After four randomized runs, which took a couple of days, I produced the 
file [log/parsed_data.txt](log/parsed_data.txt) with

```
$ ./parselog.py log/* > log/parsed_data.txt
```

## Preliminary Summary 

* In practice we
saw current wander between 10 mA and 38 mA with stabilized 3V voltage,
corresponding to 30 mW .. 114 mW range. Algorithms were clocked at
96 MHz; cycle timing was used together with integrated average energy
of each primitive to derive an energy profile for each tested algorithm.

* I can usually tell what algorithm you're running based on your wattage alone!
Power consumption is not constant, but is largely dependent on the 
instruction mix of the particular algorithm being tested. 
Very consistently and unexpectedly e.g. the NTRU key generation
routine requires only half of the wattage of decapsulation of the same
algorithm.

* I did four randomized trials for each target algorithm, 
running each component for at least 10 seconds in each (typically
tens or hundreds of iterations), and the results
are quite consistent. You may look at the semi-processed data 
[log/parsed_data.txt](log/parsed_data.txt) if you like. 

### Signature Energy

Sorted by increasing verify energy.

| Algorithm	           | KeyGen     | Sign       | Verify     |     TOTAL  |
| -------------------- | ---------: | ---------: | ---------: | ---------: |
| Falcon-512-tree      | 130.429 mJ |  11.401 mJ | 342.666 μJ | 141.292 mJ | 
| Falcon-512           | 118.671 mJ |  23.045 mJ | 345.049 μJ | 141.481 mJ | 
| Falcon-1024          | 232.567 mJ |  45.192 mJ | 690.020 μJ | 279.593 mJ | 
| Dilithium2           |   1.073 mJ |   3.777 mJ |   1.121 mJ |   5.969 mJ | 
| Dilithium3           |   1.760 mJ |   5.746 mJ |   1.705 mJ |   9.216 mJ | 
| Dilithium4           |   2.389 mJ |   5.834 mJ |   2.436 mJ |  10.659 mJ | 
| ECDSA-secp256k1      |   2.741 mJ |   3.028 mJ |   3.078 mJ |   8.849 mJ | 
| ECDSA-secp256r1      |   3.582 mJ |   3.864 mJ |   4.142 mJ |  11.578 mJ | 

### KEM Energy

Sorted by increasing total ("key exchange") energy.

| Algorithm	           | KeyGen     | Encaps     | Decaps     |     TOTAL  |
| -------------------- | ---------: | ---------: | ---------: | ---------: |
| R5ND_1KEM_0d-sneik   | 141.996 μJ | 217.931 μJ | 249.568 μJ | 609.964 μJ | 
| R5ND_1KEM_5d-sneik   | 173.318 μJ | 283.319 μJ | 365.496 μJ | 822.500 μJ | 
| R5ND_1KEM_0d         | 232.057 μJ | 353.552 μJ | 414.005 μJ | 997.273 μJ | 
| NewHope512-CPAKEM    | 407.261 μJ | 566.692 μJ |  73.184 μJ |   1.046 mJ | 
| R5ND_1KEM_5d         | 263.486 μJ | 407.138 μJ | 527.939 μJ |   1.199 mJ | 
| BabyBearEphem        | 486.618 μJ | 620.858 μJ | 195.841 μJ |   1.303 mJ | 
| Kyber512             | 413.565 μJ | 533.953 μJ | 508.782 μJ |   1.456 mJ | 
| LightSaber           | 393.189 μJ | 559.310 μJ | 587.781 μJ |   1.540 mJ | 
| R5ND_3KEM_5d-sneik   | 345.666 μJ | 597.463 μJ | 724.382 μJ |   1.668 mJ | 
| NewHope512-CCAKEM    | 483.852 μJ | 750.133 μJ | 733.988 μJ |   1.966 mJ | 
| BabyBear             | 486.745 μJ | 598.762 μJ | 904.495 μJ |   1.990 mJ | 
| NewHope1024-CPAKEM   | 802.005 μJ |   1.111 mJ | 132.250 μJ |   2.044 mJ | 
| R5ND_3KEM_5d         | 493.941 μJ | 749.414 μJ | 945.253 μJ |   2.186 mJ | 
| MamaBearEphem        | 948.591 μJ |   1.115 mJ | 266.277 μJ |   2.330 mJ | 
| R5ND_3KEM_0d-sneik   | 520.562 μJ | 845.996 μJ |   1.047 mJ |   2.416 mJ | 
| Kyber768             | 772.271 μJ | 920.445 μJ | 876.267 μJ |   2.568 mJ | 
| Saber                | 771.151 μJ |   1.008 mJ |   1.051 mJ |   2.829 mJ | 
| R5ND_5KEM_5d-sneik   | 662.612 μJ |   1.041 mJ |   1.309 mJ |   2.997 mJ | 
| R5ND_3KEM_0d         | 718.645 μJ |   1.029 mJ |   1.316 mJ |   3.066 mJ | 
| R5ND_5KEM_0d-sneik   | 754.248 μJ |   1.201 mJ |   1.491 mJ |   3.454 mJ | 
| MamaBear             | 941.423 μJ |   1.084 mJ |   1.518 mJ |   3.552 mJ | 
| R5ND_5KEM_5d         | 850.040 μJ |   1.227 mJ |   1.571 mJ |   3.647 mJ | 
| PapaBearEphem        |   1.565 mJ |   1.762 mJ | 338.695 μJ |   3.666 mJ | 
| NewHope1024-CCAKEM   | 959.295 μJ |   1.456 mJ |   1.436 mJ |   3.847 mJ | 
| R5ND_5KEM_0d         | 930.329 μJ |   1.348 mJ |   1.730 mJ |   4.006 mJ | 
| Kyber1024            |   1.265 mJ |   1.442 mJ |   1.383 mJ |   4.090 mJ | 
| FireSaber            |   1.241 mJ |   1.533 mJ |   1.596 mJ |   4.366 mJ | 
| PapaBear             |   1.554 mJ |   1.729 mJ |   2.299 mJ |   5.582 mJ | 
| LAC128               |   1.176 mJ |   2.043 mJ |   3.195 mJ |   6.412 mJ | 
| R5N1_1KEM_0d-sneik   |   2.578 mJ |   2.064 mJ |   2.318 mJ |   6.943 mJ | 
| R5N1_1KEM_0d         |   3.232 mJ |   3.065 mJ |   3.650 mJ |   9.948 mJ | 
| ECDH-secp256k1       |   2.799 mJ |   5.594 mJ |   2.801 mJ |  11.183 mJ | 
| R5N1_3KEM_0d-sneik   |   4.228 mJ |   4.111 mJ |   4.447 mJ |  12.767 mJ | 
| ECDH-secp256r1       |   3.663 mJ |   7.267 mJ |   3.656 mJ |  14.489 mJ | 
| R5N1_3KEM_0d         |   5.063 mJ |   4.987 mJ |   5.735 mJ |  15.801 mJ | 
| LAC192               |   3.916 mJ |   5.267 mJ |   9.698 mJ |  18.827 mJ | 
| LAC256               |   4.079 mJ |   7.158 mJ |  11.709 mJ |  22.971 mJ | 
| NTRU-HPS2048509      |  34.424 mJ | 395.641 μJ | 479.611 μJ |  35.238 mJ | 
| R5N1_5KEM_0d-sneik   |  18.436 mJ |  11.670 mJ |  12.695 mJ |  42.801 mJ | 
| R5N1_5KEM_0d         |  20.821 mJ |  13.616 mJ |  16.095 mJ |  50.360 mJ | 
| NTRU-HPS2048677      |  60.625 mJ | 582.792 μJ | 739.031 μJ |  61.850 mJ | 
| NTRU-HRSS701         |  66.152 mJ | 328.829 μJ | 776.016 μJ |  67.220 mJ | 
| NTRU-HPS4096821      |  90.313 mJ | 731.073 μJ | 949.618 μJ |  91.983 mJ | 
| FrodoKEM-640-AES     |  37.279 mJ |  35.083 mJ |  34.522 mJ | 106.753 mJ | 
| ntrulpr653           |  27.734 mJ |  55.236 mJ |  82.591 mJ | 164.570 mJ | 
| FrodoKEM-640-SHAKE   |  65.872 mJ |  64.942 mJ |  64.416 mJ | 195.112 mJ | 
| ntrulpr761           |  38.117 mJ |  76.138 mJ | 112.341 mJ | 225.966 mJ | 
| ntrulpr857           |  47.638 mJ |  95.158 mJ | 141.930 mJ | 282.490 mJ | 
| sntrup653            | 290.085 mJ |  28.205 mJ |  83.343 mJ | 401.405 mJ | 
| sntrup761            | 392.530 mJ |  37.841 mJ | 112.584 mJ | 546.108 mJ | 
| sntrup857            | 499.477 mJ |  48.632 mJ | 141.440 mJ | 720.013 mJ | 
| SIKEp434             | 471.009 mJ | 774.717 mJ | 826.136 mJ |   2.069  J | 
| SIKEp503             | 718.763 mJ |   1.187  J |   1.266  J |   3.162  J | 
| SIKEp610             |   1.346  J |   2.491  J |   2.505  J |   6.317  J | 
| SIKEp751             |   2.403  J |   3.934  J |   4.237  J |  10.479  J | 

**NB.** The SIKE energy numbers may be little off if the power consumption
drifts during processing (average power measurements is based only on first 10 
seconds but SIKE doesn't necessarily manage even a single iteration during
that time).

There's a faster Cortex M4 SIKE implementation reported in 
[IACR ePrint 2019/535](https://eprint.iacr.org/2019/535), but the authors
have not released their software so independent measurements are not possible. 
This improved implementation still has a reported latency of several seconds, 
so its power consumption can still be assumed to be hundreds of times higher
than with most other candidates (if not thousands as is currently the case.)

### NOTE

(c) 2019  PQShield Ltd. No warranty whatsoever; use at your own risk.

