//	randombytes.c
//	2019-09-03	Markku-Juhani O. Saarinen <mjos@pqshield.com>

#include <stdint.h>
#include <stddef.h>

#include "randombytes.h"

//	really super duper quick-and-dirty (but deterministic) fibonacci

uint32_t fibo_a = 0xDEADBEEF, fibo_b = 0x01234567;

int randombytes(uint8_t *obuf, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++) {
		obuf[i] = (fibo_a >> 24) ^ (fibo_b >> 16);
		fibo_a += fibo_b;
		fibo_b += fibo_a;
	}

	return 0;
}

