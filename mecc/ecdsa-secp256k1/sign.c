//	sign.c
//	2019-10-09	Markku-Juhani O. Saarinen <mjos@pqhsield.com>

//	NIST PQC API interface for Ken MacKay's micro-ecc

#include <stdio.h>
#include <string.h>

#include "api.h"
#include "uECC.h"
#include "randombytes.h"

#if uECC_BYTES == 32
#include "sha2.h"
#define MY_SIGN_HASH(out, in, inlen) sha256(out, in, inlen);
#endif

#ifndef MY_SIGN_HASH
#error "No a hash algorithm mathing uECC_BYTES defined"
#endif

//	uECC needs the RNG to return 1 on success

int fake_rng(uint8_t *dest, unsigned len) 
{
	randombytes(dest, len);
    return 1;
}

//	create a public-secret keypair

int crypto_sign_keypair(uint8_t *pk, uint8_t *sk)
{
    uECC_set_rng(&fake_rng);

	if (!uECC_make_key(pk, sk))
		return -1;

	return 0;
}

//	sign a message

int crypto_sign(uint8_t *sm, size_t *smlen,
    			const uint8_t *m, size_t mlen,
    			const uint8_t *sk)
{
	size_t i;
	uint8_t hash[uECC_BYTES];

    uECC_set_rng(&fake_rng);

	//	hash the message
	MY_SIGN_HASH(hash, m, mlen);

	//	compute signature
	if (!uECC_sign(sk, hash, sm))
		return -1;

	//	copy message
	*smlen = CRYPTO_BYTES + mlen;
	for (i = 0; i < mlen; i++)
		sm[CRYPTO_BYTES + i] = m[i];

	return 0;
}

//	verify a signature

int crypto_sign_open(uint8_t *m, size_t *mlen,
					 const uint8_t *sm, size_t smlen,
					 const uint8_t *pk)
{
	size_t i, ml;
	uint8_t hash[uECC_BYTES];

	//	message length
	if (smlen < CRYPTO_BYTES)
		return -1;
	ml = smlen - CRYPTO_BYTES;

	//	hash the message
	MY_SIGN_HASH(hash, &sm[CRYPTO_BYTES], ml);

	//	verify signature
	if (!uECC_verify(pk, hash, sm))	
		return -1;

	//	copy message
    *mlen = ml;
	for (i = 0; i < ml; i++)
		m[i] = sm[CRYPTO_BYTES + i];

	return 0;
}

