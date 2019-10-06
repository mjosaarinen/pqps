#!/bin/bash

for i in {1..9}
do
	./do-part crypto_aead aes128gcmv1
	cat bench/pad/data >> ../data.aes.$i
	./do-part crypto_aead aes256gcmv1
	cat bench/pad/data >> ../data.aes.$i

	./do-part crypto_stream aes128ctr
	cat bench/pad/data >> ../data.aes.$i
	./do-part crypto_stream aes256ctr
	cat bench/pad/data >> ../data.aes.$i
	./do-part crypto_stream chacha20
	cat bench/pad/data >> ../data.c20.$i

	./do-part crypto_hash sha256
	cat bench/pad/data >> ../data.sha.$i
	./do-part crypto_hash sha384
	cat bench/pad/data >> ../data.sha.$i
	./do-part crypto_hash sha512
	cat bench/pad/data >> ../data.sha.$i
	./do-part crypto_hash sha3256
	cat bench/pad/data >> ../data.sha.$i
	./do-part crypto_hash sha3384
	cat bench/pad/data >> ../data.sha.$i
	./do-part crypto_hash sha3512
	cat bench/pad/data >> ../data.sha.$i

	./do-part crypto_kem
	cat bench/pad/data >> ../data.kem.$i
	./do-part crypto_sign
	cat bench/pad/data >> ../data.sgn.$i
	./do-part crypto_encrypt
	cat bench/pad/data >> ../data.pke.$i
done

