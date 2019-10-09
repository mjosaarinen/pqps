//	api.h
//	2019-10-09	Markku-Juhani O. Saarinen <mjos@pqhsield.com>

//	NIST PQC API Interface

#ifndef _API_H_
#define _API_H_

#include <stdint.h>
#include <stddef.h>

#define CRYPTO_SECRETKEYBYTES   32
#define CRYPTO_PUBLICKEYBYTES   64
#define CRYPTO_BYTES            64
#define CRYPTO_ALGNAME          "ECDSA-secp256r1"

int crypto_sign_keypair(uint8_t *pk, uint8_t *sk);

int crypto_sign(uint8_t *sm, size_t *smlen,
    			const uint8_t *m, size_t mlen,
    			const uint8_t *sk);

int crypto_sign_open(uint8_t *m, size_t *mlen,
					 const uint8_t *sm, size_t smlen,
					 const uint8_t *pk);

#endif /* _API_H_ */
