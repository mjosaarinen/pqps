#include <stdlib.h>
#include "kernelrandombytes.h"
#include "cpucycles.h"
#include "crypto_stream.h"
#include "measure.h"

const char *primitiveimplementation = crypto_stream_IMPLEMENTATION;
const char *implementationversion = crypto_stream_VERSION;
const char *sizenames[] = { "keybytes", "noncebytes", 0 };
const long long sizes[] = { crypto_stream_KEYBYTES, crypto_stream_NONCEBYTES };

#define MAXTEST_BYTES 4096
//#ifdef SUPERCOP
//#define MGAP 8192
//#else

// XXX mjos
#define MGAP 3
//#endif

static unsigned char *k;
static unsigned char *n;
static unsigned char *m;
static unsigned char *c;

void preallocate(void)
{
}

void allocate(void)
{
  k = alignedcalloc(crypto_stream_KEYBYTES);
  n = alignedcalloc(crypto_stream_NONCEBYTES);
  m = alignedcalloc(MAXTEST_BYTES);
  c = alignedcalloc(MAXTEST_BYTES);
}

#define TIMINGS 15
static long long cycles[TIMINGS + 1];

void measure(void)
{
  int i;
  int loop;
  int mlen;

	ENERGY_VARS

  for (loop = 0;loop < LOOPS;++loop) {
    for (mlen = 0;mlen <= MAXTEST_BYTES;mlen += 1 + mlen / MGAP) {
      kernelrandombytes(k,crypto_stream_KEYBYTES);
      kernelrandombytes(n,crypto_stream_NONCEBYTES);
      kernelrandombytes(m,mlen);
      kernelrandombytes(c,mlen);

		ENERGY_BEGIN

      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_stream(c,mlen,n,k);
      }

		ENERGY_END 

     for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"cycles",cycles,TIMINGS);

		printentry(mlen, "stream_njoule", &nj, 1);
		printentry(mlen, "stream_avgcyc", &cyc, 1);

		ENERGY_BEGIN

      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_stream_xor(c,m,mlen,n,k);
      }

		ENERGY_END 

      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"xor_cycles",cycles,TIMINGS);
 
		printentry(mlen, "encdec_njoule", &nj, 1);
		printentry(mlen, "encdec_avgcyc", &cyc, 1);
   }
  }
}
