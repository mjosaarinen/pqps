#ifndef measure_h
#define measure_h

extern void printentry(long long,const char *,long long *,long long);
extern unsigned char *alignedcalloc(unsigned long long);
extern const char *primitiveimplementation;
extern const char *implementationversion;
extern const char *sizenames[];
extern const long long sizes[];
extern void preallocate(void);
extern void allocate(void);
extern void measure(void);

//	XXX mjos
long long energy_uj();

#define ENERGY_VARS					\
  long long n, uj, cc0, cyc;

#define ENERGY_BEGIN	 			\
	n = 0;							\
	uj = energy_uj();				\
	cc0 = cpucycles();				\
	do {

#define ENERGY_END					\
		n += TIMINGS + 1;			\
		cyc = cpucycles() - cc0;	\
	} while (cyc < 1000000000);		\
	uj = (energy_uj() - uj) / n;	\
	cyc /= n;

#endif
