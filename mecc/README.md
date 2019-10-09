# mecc

2019-10-09	Markku-Juhani O. Saarinen <mjos@pqshield.com>

Some NIST PQC API wrappers for Kenneth MacKay's 
[micro-ecc](https://github.com/kmackay/micro-ecc).

| Name | Type |
|:----:|:----:|
| [ecdsa-secp256k1](ecdsa-secp256k1) | ECDSA (sign), 256-bit Koblitz curve. |
| [ecdsa-secp256r1](ecdsa-secp256r1) | ECDSA (sign), 256-bit Random curve.  |
| [ecdh-secp256k1](ecdh-secp256k1) 	 | ECDH (as KEM), 256-bit Koblitz curve.|
| [ecdh-secp256r1](ecdh-secp256r1)   | ECDH (as KEM), 256-bit Random curve. |

To compile, first clone the "static" branch of micro-ecc into this
directory (`mecc`):
```
git clone -b static https://github.com/kmackay/micro-ecc.git
```

You can then test the signature algorithms and KEMs from pqps (parent 
directory) normally, with something like
```
$ make clean
$ make PQALG=mecc/ecdsa-secp256r1 flash
```
Or use the measurement script if a PowerShield is present::
```
$ ./test_alg.sh mecc/ecdsa-secp256r1
```

My power and energy measurements were:

```
ECDH-secp256k1 (sk=32, pk=64, ct=64, ss=32)
KeyGen:   4.108 Mcycles	 42.795 ms	 65.429 mW	  2.800 mJ
Encaps:   8.215 Mcycles	 85.577 ms	 65.374 mW	  5.595 mJ
Decaps:   4.108 Mcycles	 42.791 ms	 65.310 mW	  2.795 mJ
TOTAL:   16.432 Mcycles	171.163 ms	 65.363 mW	 11.188 mJ

ECDH-secp256r1 (sk=32, pk=64, ct=64, ss=32)
KeyGen:   5.814 Mcycles	 60.567 ms	 60.461 mW	  3.662 mJ
Encaps:  11.630 Mcycles	121.143 ms	 60.004 mW	  7.269 mJ
Decaps:   5.815 Mcycles	 60.572 ms	 60.328 mW	  3.654 mJ
TOTAL:   23.259 Mcycles	242.281 ms	 59.823 mW	 14.494 mJ

ECDSA-secp256k1 (sk=32, pk=64, sl=64)
KeyGen:   4.109 Mcycles	 42.806 ms	 64.082 mW	  2.743 mJ
Sign:     4.475 Mcycles	 46.616 ms	 64.925 mW	  3.027 mJ
Verify:   4.546 Mcycles	 47.351 ms	 65.018 mW	  3.079 mJ
TOTAL:   13.130 Mcycles	136.773 ms	 64.683 mW	  8.847 mJ

ECDSA-secp256r1 (sk=32, pk=64, sl=64)
KeyGen:   5.814 Mcycles	 60.565 ms	 59.129 mW	  3.581 mJ
Sign:     6.185 Mcycles	 64.428 ms	 59.954 mW	  3.863 mJ
Verify:   6.639 Mcycles	 69.160 ms	 59.826 mW	  4.138 mJ
TOTAL:   18.639 Mcycles	194.153 ms	 59.703 mW	 11.592 mJ
```

Cheers,
- markku


