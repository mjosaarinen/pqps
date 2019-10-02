# pqps/suppercop

This is a brief discussion about modding the
[supercop](https://bench.cr.yp.to/supercop.html) cryptographic benchmark to
perform energy measurements in addition to cycle counts (performance).
In other words, **SUPPERCOP** is a SUPERCOP that counts calories! 
(easy conversion: 1 J = 0.23890295761862 cal) 

**THIS IS AN EXPERIMENTAL AND COMPLETELY DJB-UNAPPROVED MOD**

Hm. We could expand the acronym (due to Paul Bakker) as 
"**S**ystem for **U**nified **P**erformance and **P**ower 
**E**valuation **R**elated to **C**ryptographic **O**perations and 
**P**rimitives". But probably won't and this will remain as a one-off.


## How

Here I'm using Intel's Running Average Power Limit (RAPL) built-in energy
measurement feature. It's available in most recent CPUs (both my personal
laptop and desktop systems have it, and they're not particularly new
or expensive).

RAPL updates the energy counters approximately every 1ms, which 
is quite coarse (few million cycles). Furthermore access is performed
via the kernel virtual filesystem (more specifically the file 
`/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj`), which creates some 
latency. Hence we are currently reading the counters before and after iterating 
the target function for at least 1,000,000,000 cycles, and calculating
an average by dividing with the number of iterations. 

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
curl https://bench.cr.yp.to/supercop/supercop-20190910.tar.xz | tar xfvJ -
cd supercop-20190910
cp ../measure.h include
cp ../measure-anything.c .
cp ../crypto_kem/measure.c crypto_kem
cp ../crypto_sign/measure.c crypto_sign
cp ../crypto_encrypt/measure.c crypto_encrypt
```
That's it. Modify to your own taste.

For actually running benchmarks you will want to look at the 
[tips](https://bench.cr.yp.to/tips.html); a full run takes a really, really
long time so I suggest severely limiting the number of compilers etc.


### Results

There's a little script `read_data.py` that I used as a basis for a parser. 

I'm currently only gathering usable results, but you can see 
[pke_example.txt](example/pke_example.txt) and
[kem_example.txt](example/kem_example.txt) for examples of some algorithms
measured under a single configuration, sorted by nanojoules/cycle
(e.g. `./read_data.py < data.kem | sort -n -k 7 > kem_example.txt`).


### Notes

The official source of information on RAPL is Volume 3 of 
[Intel® 64 and IA-32 Architectures Software Developer Manual](https://software.intel.com/en-us/articles/intel-sdm).
However this is really low-level and you should be accessing the power 
measurement/managemenent features through your operating system instead anyway.

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

You need do know what you're doing and how to properly interpret the results.

**NO WARRANTY WHATSOEVER**
