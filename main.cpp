//	main.cpp
//	2019-09-03	Markku-Juhani O. Saarinen <mjos@pqshield.com>

//	Simple testing main for algorithms.

#include "mbed.h"
#include "stm32f4xx_hal.h"


/*
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);
int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk);
int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);


int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);
int crypto_sign(unsigned char *sm, size_t *smlen,
        const unsigned char *m, size_t mlen,
        const unsigned char *sk);

int crypto_sign_open(unsigned char *m, size_t *mlen,
        const unsigned char *sm, size_t smlen,
        const unsigned char *pk);

*/


extern "C" {
#include "api.h"
}

#ifdef CRYPTO_CIPHERTEXTBYTES
#define TEST_KEM
#else
#define TEST_SIGN
#define MESSAGE_BYTES sizeof(uint64_t) 
#endif

//	mbed

Serial pc(SERIAL_TX, SERIAL_RX);
DigitalOut myled(LED1);
DigitalOut pin_d7(D7);

Timer timer;

void hexbytes(const uint8_t *data, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		pc.printf("%02X", data[i]);
	pc.printf("\n");
}

#ifdef TEST_SIGN


int test_sign()
{
	int 	i, r;

	uint8_t m[MESSAGE_BYTES];
	uint8_t pk[CRYPTO_PUBLICKEYBYTES];
	uint8_t sk[CRYPTO_SECRETKEYBYTES];
	uint8_t sm[CRYPTO_BYTES + MESSAGE_BYTES];
	size_t sm_len = 0;
	size_t m_len = MESSAGE_BYTES;

#ifdef CRYPTO_ALGNAME
	pc.printf("CRYPTO_ALGNAME          %s\n", CRYPTO_ALGNAME);
#endif        
	pc.printf("CRYPTO_SECRETKEYBYTES   %d\n", CRYPTO_SECRETKEYBYTES);
	pc.printf("CRYPTO_PUBLICKEYBYTES   %d\n", CRYPTO_PUBLICKEYBYTES);
	pc.printf("CRYPTO_BYTES            %d\n", CRYPTO_BYTES);

	timer.reset();
	timer.start();

	for (i = 0; i < 100; i++) {

		memset(m, 0x11 + i, sizeof(m));
		memset(pk, 0x22 + i, sizeof(pk));
		memset(sk, 0x33 + i, sizeof(sk));
		memset(sm, 0x44 + i, sizeof(sm));

		//	keygen
		r = crypto_sign_keypair(pk, sk);
		if (r != 0) {
			pc.printf("crypto_sign_keypair() returned %d\n", r);
			return r;
		}

		//	sign message
		r = crypto_sign(sm, &sm_len, m, MESSAGE_BYTES, sk);
		if (r != 0) {
			pc.printf("crypto_sign() returned %d\n", r);
			return r;
		}
		if (sm_len > CRYPTO_BYTES + MESSAGE_BYTES) {
			pc.printf("(Signature of %d bytes is %d bytes.)\n", 
				MESSAGE_BYTES, sm_len);
		}

		//	verify message
		r = crypto_sign_open(m, &m_len, sm, sm_len, pk);
		if (r != 0) {
			pc.printf("crypto_sign_open() returned %d\n", r);
			return r;
		}

		//	try to verify corrupt message
		sm[0]++;
		r = crypto_sign_open(m, &m_len, sm, sm_len, pk);
		if (r == 0) {
			pc.printf("crypto_sign_open() success for wrong message %d\n", r);
			return r;
		}

		//	timeout
		if (timer.read_ms() > 1000)
			break;
	}
	
	timer.stop();

	pc.printf("(self-test ok, i=%d)\n", i);

	return 0;
}

int measure_sign(int ms_time)
{
	int r;
	uint32_t t;

	uint8_t m[MESSAGE_BYTES];
	uint8_t pk[CRYPTO_PUBLICKEYBYTES];
	uint8_t sk[CRYPTO_SECRETKEYBYTES];
	uint8_t sm[CRYPTO_BYTES + MESSAGE_BYTES];
	size_t sm_len = 0;
	size_t m_len = MESSAGE_BYTES;

	uint64_t n, kg, sg, so;


	memset(m, 0x11, sizeof(m));
	memset(pk, 0x22, sizeof(pk));
	memset(sk, 0x33, sizeof(sk));
	memset(sm, 0x44, sizeof(sm));

	test_sign();

	pc.printf("\nRunning for at least %d milliseconds.\n", ms_time);

	r = 0;
	n = kg = sg = so = 0;

	timer.reset();
	timer.start();

	pin_d7 = !pin_d7;				//	trigger measurement

	do {
		n++;
		memcpy(m, &n, sizeof(n));

		t = DWT->CYCCNT;
		if (crypto_sign_keypair(pk, sk))
			r++;
		t = DWT->CYCCNT - t;
		kg += (uint64_t) t;

		t = DWT->CYCCNT;
		if (crypto_sign(sm, &sm_len, m, MESSAGE_BYTES, sk))
			r++;
		t = DWT->CYCCNT - t;
		sg += (uint64_t) t;

		t = DWT->CYCCNT;
		if (crypto_sign_open(m, &m_len, sm, sm_len, pk))
			r++;
		t = DWT->CYCCNT - t;
		so += (uint64_t) t;

	} while (timer.read_ms() < ms_time);

	timer.stop();

	pc.printf("n = %llu  (%d errors)\n", n, r);

	pc.printf("*** KeyGen %12llu %s\n",  kg / n, CRYPTO_ALGNAME);
	pc.printf("*** Sign   %12llu %s\n",  sg / n, CRYPTO_ALGNAME);
	pc.printf("*** Verify %12llu %s\n",  so / n, CRYPTO_ALGNAME);

	return r;
}

#endif /* TEST_SIGN */

#ifdef TEST_KEM

int test_kem()
{
	int i, r;

	uint8_t pk[CRYPTO_PUBLICKEYBYTES];
	uint8_t sk[CRYPTO_SECRETKEYBYTES];
	uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
	uint8_t s1[CRYPTO_BYTES];
	uint8_t s2[CRYPTO_BYTES];

#ifdef CRYPTO_ALGNAME
	pc.printf("CRYPTO_ALGNAME          %s\n", CRYPTO_ALGNAME);
#endif        
	pc.printf("CRYPTO_SECRETKEYBYTES   %d\n", CRYPTO_SECRETKEYBYTES);
	pc.printf("CRYPTO_PUBLICKEYBYTES   %d\n", CRYPTO_PUBLICKEYBYTES);
	pc.printf("CRYPTO_CIPHERTEXTBYTES  %d\n", CRYPTO_CIPHERTEXTBYTES);
	pc.printf("CRYPTO_BYTES            %d\n", CRYPTO_BYTES);

	timer.reset();
	timer.start();

	//	check that it works correctly
	for (i = 0; i < 100; i++) {

		memset(pk, 0x11 + i, sizeof(pk));
		memset(sk, 0x22 + i, sizeof(sk));
		memset(ct, 0x33 + i, sizeof(ct));
		memset(s1, 0x44 + i, sizeof(s1));
		memset(s2, 0x55 + i, sizeof(s2));

		r = crypto_kem_keypair(pk, sk);
		if (r != 0) {
			pc.printf("crypto_kem_keypair() returned %d\n", r);
			return r;
		}

		r = crypto_kem_enc(ct, s1, pk);
		if (r != 0) {
			pc.printf("crypto_kem_enc() returned %d\n", r);
			return r;
		}

		r = crypto_kem_dec(s2, ct, sk);
		if (r != 0) {
			pc.printf("crypto_kem_dec() returned %d\n", r);
			return r;
		}

		if (memcmp(s1, s2, CRYPTO_BYTES) != 0) {
			pc.printf("Shared secret mismatch!\n");
			pc.printf("s1 = ");
			hexbytes(s1, sizeof(s1));
			pc.printf("s2 = ");
			hexbytes(s1, sizeof(s2));
			return -1;
		}

		//	timeout
		if (timer.read_ms() > 1000)
			break;
	}
	timer.stop();

	pc.printf("(self-test ok, i=%d.)\n", i);

	return 0;
}

int measure_kem(int ms_time)
{
	int r;
	uint32_t t;

	uint8_t pk[CRYPTO_PUBLICKEYBYTES];
	uint8_t sk[CRYPTO_SECRETKEYBYTES];
	uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
	uint8_t s1[CRYPTO_BYTES];
	uint8_t s2[CRYPTO_BYTES];

	uint64_t n, kg, enc, dec;

	memset(pk, 0x11, sizeof(pk));
	memset(sk, 0x22, sizeof(sk));
	memset(ct, 0x33, sizeof(ct));
	memset(s1, 0x44, sizeof(s1));
	memset(s2, 0x55, sizeof(s2));

	//	main test
	pc.printf("\nRunning for at least %d milliseconds.\n", ms_time);

	r = 0;
	n = kg = enc = dec = 0;

	timer.reset();
	timer.start();

	pin_d7 = !pin_d7;				//	trigger measurement

	do {
		n++;

		t = DWT->CYCCNT;
		if (crypto_kem_keypair(pk, sk))
			r++;
		t = DWT->CYCCNT - t;
		kg += (uint64_t) t;

		t = DWT->CYCCNT;
		if (crypto_kem_enc(ct, s1, pk))
			r++;
		t = DWT->CYCCNT - t;
		enc += (uint64_t) t;

		t = DWT->CYCCNT;
		if (crypto_kem_dec(s2, ct, sk))
			r++;
		t = DWT->CYCCNT - t;
		dec += (uint64_t) t;

	} while (timer.read_ms() < ms_time);

	timer.stop();

	pc.printf("n = %llu  (%d errors)\n", n, r);

	pc.printf("*** KeyGen %12llu %s\n",  kg / n, CRYPTO_ALGNAME);
	pc.printf("*** Encaps %12llu %s\n",  enc / n, CRYPTO_ALGNAME);
	pc.printf("*** Decaps %12llu %s\n",  dec / n, CRYPTO_ALGNAME);

	return r;
}

#endif	/* TEST_KEM */


int main()
{
	//	downclock
	//	SystemCoreClock = 24000000;
	//	SystemCoreClockUpdate();


	pc.baud(115200);
	pc.printf("\n[RESET] This is PQPowerShield! Welcome.\n\n");
 	pc.printf("SystemCoreClock         %lu\n", SystemCoreClock); 

	//	set so that cycle counter can be read from  DWT->CYCCNT
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

#ifdef TEST_KEM
	test_kem();
	measure_kem(5000);
#endif

#ifdef TEST_SIGN
	test_sign();
	measure_sign(5000);
#endif

	pc.printf("\n[DONE] sleep()\n");

	while (1) {
		sleep();
		myled = !myled;
	}	
}
