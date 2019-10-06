# pqps/suppercop

This is a brief discussion about modding the
[supercop](https://bench.cr.yp.to/supercop.html) cryptographic benchmark to
perform energy measurements in addition to cycle counts (performance).
In other words, **SUPPERCOP** is a SUPERCOP that counts calories! 
(easy conversion: 1 cal = 4.184 J) 

**THIS IS AN EXPERIMENTAL AND COMPLETELY DJB-UNAPPROVED MOD**

Hm. We could expand the acronym (due to Paul Bakker) as 
"**S**ystem for **U**nified **P**erformance and **P**ower 
**E**valuation **R**elated to **C**ryptographic **O**perations and 
**P**rimitives". But probably won't and this will remain as a one-off.


## How

Here I'm using Intel's Running Average Power Limit (RAPL) built-in energy
measurement feature. It's available in most currently Intel CPUs (after 
"Sandy Bridge" -- roughly 2011). AMD processors (eg. Ryzen) do not seem 
to support it.

RAPL updates the energy counters approximately every 1ms, which 
is quite coarse (few million cycles). Furthermore access is performed
via the kernel virtual filesystem (more specifically the file 
`/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj`), which creates some 
latency. Hence we are currently reading the counters before and after 
iterating the target function for at least 1,000,000,000 cycles, and 
calculating an average by dividing with the number of iterations. 

The new measurements are average energy `_ujoule` and average cycles 
`_avgcyc`:

| **Primitive**		| **Energy** (μJ)	| **Time** (cycles)	|
| ----------------- | ----------------- | ----------------- |
| KEM Keypair 		| `kem_kg_ujoule`	| `kem_kg_avgcyc`	|
| KEM Encaps		| `kem_enc_ujoule`	| `kem_enc_avgcyc`	|
| KEM Decaps		| `kem_dec_ujoule`	| `kem_dec_avgcyc`	|
| PKE Keypair		| `pke_kg_ujoule`	| `pke_kg_avgcyc`	|
| PKE Encrypt		| `pke_enc_ujoule`	| `pke_enc_avgcyc`	|
| PKE Decrypt		| `pke_dec_ujoule`	| `pke_dec_avgcyc`	|
| Sign Keypair		| `sign_kg_ujoule`	| `sign_kg_avgcyc`	|
| Make signature	| `sign_do_ujoule`	| `sign_do_avgcyc`	|
| Verify signature	| `sign_vfy_ujoule`	| `sign_vfy_avgcyc`	|

Sorry for diverging from the supercop primitive operation naming convention, 
but this happened to be better for my simple parsing scripts.


### The Mod

You can fetch supercop into this directory( `pqps/supercop`), simply 
replace the relevant files with ones supplied here, and run it. Check the 
current version [here](https://bench.cr.yp.to/supercop.html) -- replacing 
the date/version string in the following:

```
curl https://bench.cr.yp.to/supercop/supercop-20190910.tar.xz | tar xfJ -
cd supercop-20190910
cp ../include/measure.h include
cp ../measure-anything.c .
cp ../crypto_kem/measure.c crypto_kem/measure.c
cp ../crypto_sign/measure.c crypto_sign/measure.c
cp ../crypto_encrypt/measure.c crypto_encrypt/measure.c
```
That's it. Modify to your own taste. The default supercop configuration
is to test public key encryption and digital signatures for a range of
message sizes; however this isn't terribly useful since most schemes
that just sign a hash or encrypt a symmetric key. There are variants
`measure.c.mlen16` that just fix the random payload at 16 bytes in the 
directories. You can copy those instead, speeding up testing quite a bit.

```
cp ../crypto_sign/measure.c.mlen16 crypto_sign/measure.c
cp ../crypto_encrypt/measure.c.mlen16 crypto_encrypt/measure.c
```

For actually running benchmarks you will want to look at the 
[tips](https://bench.cr.yp.to/tips.html); a full run takes a really, really
long time so I suggest severely limiting the number of compilers etc.


### Results

There's a little script [read_data.py](read_data.py) that I used as a basis 
for a parser. 

I'm currently only gathering usable results, but you can see 
[blk_test.txt](example/blk_test.txt) and [pad_test.txt](example/pad_test.txt)
for examples of algorithms measured under two different configurations ("blk" 
is a Core i7-8700 - 3.20GHz desktop system while "pad" is a Core i5-8250U 
CPU - 1.60GHz laptop). They were generated from a bunch of data files with:
```
cat data* | ./read_data.py | sort > example/xxx_test.txt
```
Not all algorithms were enabled in these runs.

### Notes

The official source of information on RAPL is Volume 3 of 
[Intel® 64 and IA-32 Architectures Software Developer Manual](https://software.intel.com/en-us/articles/intel-sdm).
However this is really low-level.

For practical discussion on the matter, I suggest getting hold of a this paper 
by K.N. Khan, M. Hirki, T. Niemi, J.K. Nurminen, Z. Ou:
[RAPL in Action: Experiences in Using RAPL for Power Measurements](https://doi.org/10.1145/3177754), 
published TOMPECS / ICPE 2017, April 2018. 

As noted in Vince Weaver's excellent
[RAPL web page](http://web.eece.maine.edu/~vweaver/projects/rapl/) there
are three ways getting to the same data in Linux, only one of which is 
permissionless, the powercap interface (the one I'm using). Official, rather
limited Linux kernel documentation of "Power Capping Framework" is contained in
[powercap.txt](https://www.kernel.org/doc/Documentation/power/powercap/powercap.txt).

## Caveats

You need to know what you're doing and how to properly interpret the results.

**NO WARRANTY WHATSOEVER**
