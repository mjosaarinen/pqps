//	main.cpp
//	2019-09-03	Markku-Juhani O. Saarinen <mjos@pqshield.com>

//	Simple testing main for algorithms.

#include "mbed.h"
#include "stm32f4xx_hal.h"

extern "C" {
#include "api.h"
}

/*

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);
int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk);
int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);

*/

//	mbed

Serial pc(SERIAL_TX, SERIAL_RX);
DigitalOut myled(LED1);
Timer timer;

void hexbytes(const uint8_t *data, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		pc.printf("%02X", data[i]);
	pc.printf("\n");
}

int test_kem(int ms_time)
{
	int r;
	uint32_t t;

	uint8_t pk[CRYPTO_PUBLICKEYBYTES];
	uint8_t sk[CRYPTO_SECRETKEYBYTES];
	uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
	uint8_t s1[CRYPTO_BYTES];
	uint8_t s2[CRYPTO_BYTES];

	uint64_t n, kg, enc, dec;

#ifdef CRYPTO_ALGNAME
	pc.printf("CRYPTO_ALGNAME          %s\n", CRYPTO_ALGNAME);
#endif        
	pc.printf("CRYPTO_SECRETKEYBYTES   %d\n", CRYPTO_SECRETKEYBYTES);
	pc.printf("CRYPTO_PUBLICKEYBYTES   %d\n", CRYPTO_PUBLICKEYBYTES);
	pc.printf("CRYPTO_CIPHERTEXTBYTES  %d\n", CRYPTO_CIPHERTEXTBYTES);
	pc.printf("CRYPTO_BYTES            %d\n", CRYPTO_BYTES);

	memset(pk, 0x11, sizeof(pk));
	memset(sk, 0x22, sizeof(sk));
	memset(ct, 0x33, sizeof(ct));
	memset(s1, 0x44, sizeof(s1));
	memset(s2, 0x55, sizeof(s2));

	pc.printf("\nRunning for at least %d milliseconds.\n", ms_time);

	n = kg = enc = dec = 0;

	timer.start();
	timer.reset();

	do {

		n++;
		myled = !myled;

		t = DWT->CYCCNT;
		r = crypto_kem_keypair(pk, sk);
		t = DWT->CYCCNT - t;
		kg += (uint64_t) t;

		if (r != 0) {
			printf("crypto_kem_keypair() returned %d\n", r);
			return r;
		}

		t = DWT->CYCCNT;
		r = crypto_kem_enc(ct, s1, pk);
		t = DWT->CYCCNT - t;
		enc += (uint64_t) t;

		if (r != 0) {
			printf("crypto_kem_enc() returned %d\n", r);
			return r;
		}

		t = DWT->CYCCNT;
		r = crypto_kem_dec(s2, ct, sk);
		t = DWT->CYCCNT - t;
		dec += (uint64_t) t;

		if (memcmp(s1, s2, CRYPTO_BYTES) != 0) {
			printf("Shared secret mismatch!\n");
			printf("s1 = ");
			hexbytes(s1, sizeof(s1));
			printf("s2 = ");
			hexbytes(s1, sizeof(s2));
		}
	} while (timer.read_ms() < ms_time);

	timer.stop();

	pc.printf("n = %llu\n", n);

	pc.printf("*** KeyGen %12llu %s\n",  kg / n, CRYPTO_ALGNAME);
	pc.printf("*** Encaps %12llu %s\n",  enc / n, CRYPTO_ALGNAME);
	pc.printf("*** Decaps %12llu %s\n",  dec / n, CRYPTO_ALGNAME);

	return 0;
}

int main()
{
	//	downclock
	//	SystemCoreClock = 24000000;
	//	SystemCoreClockUpdate();

	pc.printf("\n[RESET] This is PQPowerShield! Welcome.\n\n");

 	pc.printf("SystemCoreClock         %lu\n", SystemCoreClock); 

	//	set so that cycle counter can be read from  DWT->CYCCNT
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	test_kem(5000);

	pc.printf("\n[DONE] sleep()\n");

	while (1) {
		sleep();
		myled = !myled;
	}	
}
