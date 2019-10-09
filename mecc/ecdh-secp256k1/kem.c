//	sign.c
//	2019-10-09	Markku-Juhani O. Saarinen <mjos@pqhsield.com>

//	NIST PQC API interface for Ken MacKay's micro-ecc

#include <stdio.h>
#include <string.h>

#include "api.h"
#include "uECC.h"
#include "randombytes.h"

//	uECC needs the RNG to return 1 on success

int fake_rng(uint8_t *dest, unsigned len) 
{
	randombytes(dest, len);

    return 1;
}

//	create a public-secret keypair

int crypto_kem_keypair(uint8_t *pk, uint8_t *sk)
{
	uECC_set_rng(&fake_rng);

	//	alice: pk = g*a, sk = a
	if (!uECC_make_key(pk, sk))
		return -1;

	return 0;
}

//	encapsulate

int crypto_kem_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk)
{
	uint8_t tk[uECC_BYTES];

	uECC_set_rng(&fake_rng);

	//	bob: ct = g*b, tk = b
	if (!uECC_make_key(ct, tk))
		return -1;

	//	shared secret: ss = (g*a)*b, where pk = g^a and tk = b
	if (!uECC_shared_secret(pk, tk, ss))
		return -1;

	return 0;
}

//	decapsulate

int crypto_kem_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk)
{
	//	alice: ss = (g*b)*a, where ct = g*b and sk = a
	if (!uECC_shared_secret(ct, sk, ss))
		return -1;

	return 0;
}

