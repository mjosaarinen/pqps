//	api.h
//	2019-10-09	Markku-Juhani O. Saarinen <mjos@pqhsield.com>

//	NIST PQC API Interface

#ifndef _API_H_
#define _API_H_

#include <stdint.h>

#define CRYPTO_SECRETKEYBYTES	32
#define CRYPTO_PUBLICKEYBYTES	64
#define CRYPTO_BYTES			32
#define CRYPTO_CIPHERTEXTBYTES	64
#define CRYPTO_ALGNAME          "ECDH-secp256k1"

int crypto_kem_keypair(uint8_t *pk, uint8_t *sk);

int crypto_kem_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk);

int crypto_kem_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk);

#endif /* _API_H_ */

