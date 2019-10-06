#!/bin/bash
cd supercop-20??????
cp ../include/measure.h include/measure.h
cp ../measure-anything.c .
cp ../crypto_kem/measure.c crypto_kem/measure.c
cp ../crypto_sign/measure.c.mlen16 crypto_sign/measure.c
cp ../crypto_encrypt/measure.c.mlen16 crypto_encrypt/measure.c
cp ../crypto_aead/measure.c crypto_aead/measure.c
cp ../crypto_hash/measure.c crypto_hash/measure.c
cp ../crypto_stream/measure.c crypto_stream/measure.c

