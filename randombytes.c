//	randombytes.c
//	2019-09-03	Markku-Juhani O. Saarinen <mjos@pqshield.com>

#include <stdint.h>
#include <stddef.h>

#include "randombytes.h"

//	really super duper quick-and-dirty (but deterministic) fibonacci

static uint32_t fibo_a = 0xDEADBEEF, fibo_b = 0x01234567;

//	we want to be able to quickly reset the state to some value

void my_random_seed(int seed)
{
	fibo_a = (0xDEADBEEF ^ (seed << 20) ^ (seed >> 12)) | 1;
	fibo_b = seed;
}

//	simple/stupid fibonacci generator

int randombytes(uint8_t *obuf, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++) {
		fibo_a += fibo_b;
		fibo_b += fibo_a;
		obuf[i] = (fibo_a >> 24) ^ (fibo_b >> 16);
	}

	return 0;
}

