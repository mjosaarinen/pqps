#!/bin/bash

#	qad_run.sh
#	2019-10-07	Markku-Juhani O. Saarinen <mjos@pqshield.com>

#	qad (quick-and-dirty) run script for supercop

cd supercop-20??????

# these are our benchmark targets atm

echo crypto_*/*/nistpqc2 | sed 's/\/nistpqc2//g' > test.tmp
echo crypto_aead/aes???gcmv1 \
	crypto_stream/aes???ctr crypto_stream/chacha20 \
	crypto_hash/sha??? crypto_hash/sha3??? \
	crypto_sign/ed25519 crypto_sign/ecdonaldp??? \
	crypto_encrypt/rsa2048 \
	crypto_kem/rsa2048 >> test.tmp

mkdir -p ../data

for i in {1..9}
do
	# randomize order
	algs=`cat test.tmp | tr ' /' '\n.' | uniq | shuf`
	for x in $algs
	do
		echo $x.$i
		./do-part `echo $x | tr '.' ' '`
		cp bench/$HOSTNAME/data ../data/data.$x.$i
	done
done

