#include <stdlib.h>
#include "kernelrandombytes.h"
#include "cpucycles.h"
#include "crypto_hash.h"
#include "measure.h"

const char *primitiveimplementation = crypto_hash_IMPLEMENTATION;
const char *implementationversion = crypto_hash_VERSION;
const char *sizenames[] = { "outputbytes", 0 };
const long long sizes[] = { crypto_hash_BYTES };

#define MAXTEST_BYTES 4096
//#ifdef SUPERCOP
//#define MGAP 8192
//#else
#define MGAP 3
//#endif

static unsigned char *h;
static unsigned char *m;

void preallocate(void)
{
}

void allocate(void)
{
  h = alignedcalloc(crypto_hash_BYTES);
  m = alignedcalloc(MAXTEST_BYTES);
}

#define TIMINGS 15
static long long cycles[TIMINGS + 1];

static void printcycles(long long mlen)
{
  int i;
  for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
  printentry(mlen,"cycles",cycles,TIMINGS);
}

void measure(void)
{
  int i;
  int loop;
  int mlen;

	ENERGY_VARS

  for (loop = 0;loop < LOOPS;++loop) {
    for (mlen = 0;mlen <= MAXTEST_BYTES;mlen += 1 + mlen / MGAP) {
      kernelrandombytes(m,mlen);

		ENERGY_BEGIN

      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
		crypto_hash(h,m,mlen);
      }

		ENERGY_END

      printcycles(mlen);

		printentry(mlen, "hash_njoule", &nj, 1);
		printentry(mlen, "hash_avgcyc", &cyc, 1);
    }
  }
}
