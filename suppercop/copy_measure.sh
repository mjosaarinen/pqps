#!/bin/bash

#	copy_measure.sh
#	2019-10-07	Markku-Juhani O. Saarinen <mjos@pqhshield.com>

#	prepare supercop. this is super hacky, i know

url=https://bench.cr.yp.to/`curl -s https://bench.cr.yp.to/supercop.html | \
	grep wget | grep -o '".*"' | tr -d '"' | head -1`
dir=`basename -s .tar.xz $url`

echo Fetching $dir from $url
echo "Press ^C to cancel, enter to start!"
read

rm -rf $dir
curl $url | tar xfJ -

cd $dir
cp ../include/measure.h include/measure.h
cp ../measure-anything.c .
cp ../crypto_kem/measure.c crypto_kem/measure.c
cp ../crypto_sign/measure.c.mlen16 crypto_sign/measure.c
cp ../crypto_encrypt/measure.c.mlen16 crypto_encrypt/measure.c

# remove all but first compiler

head -1 okcompilers/c > t; mv t okcompilers/c
head -1 okcompilers/cpp > t; mv t okcompilers/cpp

# ugh

./do-part init
./do-part crypto_core
./do-part keccak
rm -rf crypto_stream/chacha20/moon
./do-part crypto_stream chacha20
./do-part crypto_stream salsa20
./do-part crypto_stream aes256ctr
./do-part crypto_rng
./do-part used

# now get the symmetric stuff

cp ../crypto_aead/measure.c crypto_aead/measure.c
cp ../crypto_hash/measure.c crypto_hash/measure.c
cp ../crypto_stream/measure.c crypto_stream/measure.c

