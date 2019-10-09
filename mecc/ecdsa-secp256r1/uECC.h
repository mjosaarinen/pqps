//	uECC.h
//	2019-10-09	Markku-Juhani O. Saarinen <mjos@pqhsield.com>

//	Set defaults for Ken MacKay's micro-ecc

#ifndef _UECC_H_
#define _UECC_H_

#include "api.h"

#if (CRYPTO_SECRETKEYBYTES == 32)
//	Choose a "random" (not Koblitz) curve
#define uECC_CURVE uECC_secp256r1
#endif

#include "../micro-ecc/uECC.h"

#endif /* _UECC_H_ */

