//	main.cpp
//	2019-09-03	Markku-Juhani O. Saarinen <mjos@pqshield.com>

//	Simple testing main() for algorithms.

#include "mbed.h"
#include "stm32f4xx_hal.h"

//	mbed API forces us the be in C++, even though the code is really C
extern "C" {

//	NIST API and algorithm details
#include "api.h"

//	randombytes.c
void my_random_seed(int seed);

}

#ifdef CRYPTO_CIPHERTEXTBYTES
#define TEST_KEM
#else
#define TEST_SIGN
#define MESSAGE_BYTES sizeof(uint64_t) 
#endif

//	mbed

Serial ser(SERIAL_TX, SERIAL_RX);
DigitalOut myled(LED1);
DigitalOut pin_d7(D7);
Timer timer;

//	print 

void hexbytes(const uint8_t *data, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		ser.printf("%02X", data[i]);
	ser.printf("\n");
}

//	sleepy sleep (but trigger!)

void zzz_ms(int ms_time)
{
	ser.printf("\n[ZZZ] test_kem(%d)\n", ms_time);

	timer.reset();
	timer.start();

	pin_d7 = !pin_d7;				//	trigger measurement

	do {
		sleep();
	} while (timer.read_ms() < ms_time);
	timer.stop();
}

//	==== SIGNATURE ALGORITHMS ====

#ifdef TEST_SIGN

//	signing state
static uint8_t m[MESSAGE_BYTES];
static uint8_t pk[CRYPTO_PUBLICKEYBYTES];
static uint8_t sk[CRYPTO_SECRETKEYBYTES];
static uint8_t sm[CRYPTO_BYTES + MESSAGE_BYTES];
static size_t sm_len = 0;
static size_t m_len = MESSAGE_BYTES;
static int init_sign = 0;

int test_sign(int ms_time)
{
	int 	i, r;

	ser.printf("\n[INIT] test_sign(%d)\n", ms_time);

#ifdef CRYPTO_ALGNAME
	ser.printf("CRYPTO_ALGNAME          %s\n", CRYPTO_ALGNAME);
#endif        
	ser.printf("CRYPTO_SECRETKEYBYTES   %d\n", CRYPTO_SECRETKEYBYTES);
	ser.printf("CRYPTO_PUBLICKEYBYTES   %d\n", CRYPTO_PUBLICKEYBYTES);
	ser.printf("CRYPTO_BYTES            %d\n", CRYPTO_BYTES);

	timer.reset();
	timer.start();

	for (i = 0; i < 1000; i++) {

		my_random_seed(i);

		memset(m, 0x11 + i, sizeof(m));
		memset(pk, 0x22 + i, sizeof(pk));
		memset(sk, 0x33 + i, sizeof(sk));
		memset(sm, 0x44 + i, sizeof(sm));

		//	keygen
		r = crypto_sign_keypair(pk, sk);
		if (r != 0) {
			ser.printf("i=%d: crypto_sign_keypair() returned %d\n", i, r);
			return r;
		}

		//	sign message
		r = crypto_sign(sm, &sm_len, m, MESSAGE_BYTES, sk);
		if (r != 0) {
			ser.printf("i=%d: crypto_sign() returned %d\n", i, r);
			return r;
		}
		if (sm_len > CRYPTO_BYTES + MESSAGE_BYTES) {
			ser.printf("i=%d: (Signature of %d bytes is %d bytes.)\n", 
				i, MESSAGE_BYTES, sm_len);
		}

		//	verify message
		r = crypto_sign_open(m, &m_len, sm, sm_len, pk);
		if (r != 0) {
			ser.printf("i=%d: crypto_sign_open() returned %d\n", i, r);
			return r;
		}

		//	try to verify corrupt message
		sm[0]++;
		r = crypto_sign_open(m, &m_len, sm, sm_len, pk);
		if (r == 0) {
			ser.printf("i=%d: crypto_sign_open() success for bad sign\n", i);
			return -1;
		}

		//	timeout
		if (timer.read_ms() > ms_time)
			break;
	}
	
	timer.stop();

	ser.printf("[INIT] self-test ok, i=%d\n", i);
	init_sign = 1;

	return 0;
}

int measure_sign(int ms_time, int do_kg, int do_sg, int do_so)
{
	int r, ms;
	uint32_t t;

	uint64_t n, kg, sg, so;

	//	make sure that there is a keypair
	if (!init_sign)
		test_sign(0);

	ser.printf("\n[START] measure_sign(%d%s%s%s)\n", ms_time,
		do_kg ? ",kg" : "", do_sg ? ",sign" : "", do_so ? ",ver" : "");

	r = 0;
	n = kg = sg = so = 0;

	timer.reset();
	timer.start();

	pin_d7 = !pin_d7;				//	trigger measurement

	do {
		n++;
		memcpy(m, &n, sizeof(n));

		if (do_kg) {
			t = DWT->CYCCNT;
			if (crypto_sign_keypair(pk, sk))
				r++;
			t = DWT->CYCCNT - t;
			kg += (uint64_t) t;
		}

		if (do_sg) {
			t = DWT->CYCCNT;
			if (crypto_sign(sm, &sm_len, m, MESSAGE_BYTES, sk))
				r++;
			t = DWT->CYCCNT - t;
			sg += (uint64_t) t;
		}
	
		if (do_so) {
			t = DWT->CYCCNT;
			if (crypto_sign_open(m, &m_len, sm, sm_len, pk))
				r++;
			t = DWT->CYCCNT - t;
			so += (uint64_t) t;
		}

		ms = timer.read_ms();

	} while (ms < ms_time);

	timer.stop();

	ser.printf("*** %d milliseconds, n = %llu (%d errors)\n", ms, n, r);

	if (n == 0)
		return r;

	if (kg > 0)
		ser.printf("*** KeyGen %12llu %s\n",  kg / n, CRYPTO_ALGNAME);
	if (sg > 0)
		ser.printf("*** Sign   %12llu %s\n",  sg / n, CRYPTO_ALGNAME);
	if (so > 0)
		ser.printf("*** Verify %12llu %s\n",  so / n, CRYPTO_ALGNAME);
	ser.printf("*** Total  %12llu %s\n",  (kg+sg+so) / n, CRYPTO_ALGNAME);

	return r;
}

#endif /* TEST_SIGN */


//	==== KEY ENCAPSULATION ALGORITHMS ====

#ifdef TEST_KEM

//	KEM state
static uint8_t pk[CRYPTO_PUBLICKEYBYTES];
static uint8_t sk[CRYPTO_SECRETKEYBYTES];
static uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
static uint8_t s1[CRYPTO_BYTES];
static uint8_t s2[CRYPTO_BYTES];
static int	init_kem = 0;

int test_kem(int ms_time)
{
	int i, r;

	ser.printf("\n[INIT] test_kem(%d)\n", ms_time);

#ifdef CRYPTO_ALGNAME
	ser.printf("CRYPTO_ALGNAME          %s\n", CRYPTO_ALGNAME);
#endif        
	ser.printf("CRYPTO_SECRETKEYBYTES   %d\n", CRYPTO_SECRETKEYBYTES);
	ser.printf("CRYPTO_PUBLICKEYBYTES   %d\n", CRYPTO_PUBLICKEYBYTES);
	ser.printf("CRYPTO_CIPHERTEXTBYTES  %d\n", CRYPTO_CIPHERTEXTBYTES);
	ser.printf("CRYPTO_BYTES            %d\n", CRYPTO_BYTES);

	timer.reset();
	timer.start();

	//	check that it works correctly
	for (i = 0; i < 1000; i++) {

		my_random_seed(i);

		memset(pk, 0x11 + i, sizeof(pk));
		memset(sk, 0x22 + i, sizeof(sk));
		memset(ct, 0x33 + i, sizeof(ct));
		memset(s1, 0x44 + i, sizeof(s1));
		memset(s2, 0x55 + i, sizeof(s2));

		r = crypto_kem_keypair(pk, sk);
		if (r != 0) {
			ser.printf("i=%d: crypto_kem_keypair() returned %d\n", i, r);
			return r;
		}

		r = crypto_kem_enc(ct, s1, pk);
		if (r != 0) {
			ser.printf("i=%d: crypto_kem_enc() returned %d\n", i, r);
			return r;
		}

		r = crypto_kem_dec(s2, ct, sk);
		if (r != 0) {
			ser.printf("i=%d: crypto_kem_dec() returned %d\n", i, r);
			return r;
		}

		if (memcmp(s1, s2, CRYPTO_BYTES) != 0) {
			ser.printf("i=%d: Shared secret mismatch!\n", i);
			ser.printf("s1 = ");
			hexbytes(s1, sizeof(s1));
			ser.printf("s2 = ");
			hexbytes(s1, sizeof(s2));
			return -1;
		}

		//	timeout
		if (timer.read_ms() > ms_time)
			break;
	}
	timer.stop();

	ser.printf("[INIT] self-test ok, i=%d\n", i);

	init_kem = 1;

	return 0;
}


int measure_kem(int ms_time, int do_kg, int do_enc, int do_dec)
{
	int r, ms;
	uint32_t t;

	uint64_t n, kg, enc, dec;

	//	make sure that there is a keypair
	if (!init_kem)
		test_kem(0);

	//	main test
	ser.printf("\n[START] measure_kem(%d%s%s%s)\n", ms_time, 
		do_kg ? ",kg" : "", do_enc ? ",enc" : "", do_dec ? ",dec" : "");

	r = 0;
	n = kg = enc = dec = 0;

	timer.reset();
	timer.start();

	pin_d7 = !pin_d7;				//	trigger measurement

	do {
		n++;

		if (do_kg) {
			t = DWT->CYCCNT;
			if (crypto_kem_keypair(pk, sk))
				r++;
			t = DWT->CYCCNT - t;
			kg += (uint64_t) t;
		}

		if (do_enc) {
			t = DWT->CYCCNT;
			if (crypto_kem_enc(ct, s1, pk))
				r++;
			t = DWT->CYCCNT - t;
			enc += (uint64_t) t;
		}

		if (do_dec) {
			t = DWT->CYCCNT;
			if (crypto_kem_dec(s2, ct, sk))
				r++;
			t = DWT->CYCCNT - t;
			dec += (uint64_t) t;
		}

		ms = timer.read_ms();

	} while (ms < ms_time);

	ser.printf("[END] %d milliseconds, n=%llu (%d errors)\n", ms, n, r);
	timer.stop();

	if (n == 0)
		return r;

	if (kg > 0)
		ser.printf("*** KeyGen %12llu %s\n",  kg / n, CRYPTO_ALGNAME);
	if (enc > 0)
		ser.printf("*** Encaps %12llu %s\n",  enc / n, CRYPTO_ALGNAME);
	if (dec > 0)
		ser.printf("*** Decaps %12llu %s\n",  dec / n, CRYPTO_ALGNAME);

 	ser.printf("*** Total  %12llu %s\n",  (kg+enc+dec) / n, CRYPTO_ALGNAME);

	return r;
}

#endif	/* TEST_KEM */


int main()
{
	int ch;

	ser.baud(115200);
	ser.printf("\n[RESET] This is PQPowerShield! Welcome.\n\n");
 	ser.printf("SystemCoreClock         %lu\n", SystemCoreClock); 

	//	set so that cycle counter can be read from  DWT->CYCCNT
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

#ifdef TEST_KEM

	test_kem(1000);
	do {
		ser.printf("[INPUT] a = all, k = keygen, e = encaps, d = decaps\n");	

		switch (ch = ser.getc()) {
			case 'a':
				measure_kem(5000, 1, 1, 1);
				break;
			case 'k':
				measure_kem(1000, 1, 0, 0);
				break;
			case 'e':
				measure_kem(1000, 0, 1, 0);
				break;
			case 'd':
				measure_kem(1000, 0, 0, 1);
				break;
			case 't':
				test_kem(60000);
				break;
			case 'z':
				zzz_ms(5000);
				break;
		}
	} while (ch != 'x');

#endif

#ifdef TEST_SIGN

	test_sign(1000);
	do {
		ser.printf("[INPUT] a = all, k = keygen, s = sign, v = verify\n");	

		switch (ch = ser.getc()) {
			case 'a':
				measure_sign(5000, 1, 1, 1);
				break;
			case 'k':
				measure_sign(1000, 1, 0, 0);
				break;
			case 's':
				measure_sign(1000, 0, 1, 0);
				break;
			case 'v':
				measure_sign(1000, 0, 0, 1);
				break;
			case 't':
				test_sign(60000);
				break;
			case 'z':
				zzz_ms(5000);
				break;
		}
	} while (ch != 'x');
#endif

	ser.printf("\n[DONE] sleep()\n");

	while (1) {
		sleep();
	}	
}
