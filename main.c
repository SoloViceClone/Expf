using namespace std;

#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <bitset>

extern float expf64(float x);

extern float dummy(float x);

#define RANDTESTSIZE 0x10000

float randtest[RANDTESTSIZE];

typedef union {
	float f;
	int32_t i32;
	uint32_t ui32;
} binary32;

typedef union {
	double d;
	int64_t i64;
	uint64_t ui64;
} binary64;


int32_t i_from_f(float x) {
	binary32 b;
	b.f = x;
	return b.ui32;
}

uint64_t get_cycle_counter() {
#ifdef __k1__
    __builtin_k1_barrier();
    unsigned k = __k1_counter_num(0);
    __builtin_k1_barrier();
    return k;

#else

		uint64_t time;
		__asm__ __volatile__(															 \
          "xorl %%eax,%%eax\n\t"                       \
          "cpuid\n\t"                                  \
          "rdtsc\n\t"                                  \
          "movl %%eax,(%0)\n\t"                        \
          "movl %%edx,4(%0)\n\t"                       \
          "xorl %%eax,%%eax\n\t"                       \
          "cpuid\n\t"                                  \
          : /* nothing */                              \
          : "S"((&time))                                \
          : "eax", "ebx", "ecx", "edx", "memory");
		return time;
#endif
}

int32_t singleTest(float x) {
	float ri = expf64(x);
	float rd = (float)exp((double)x);
	int32_t ri2bin = i_from_f(ri);
	int32_t rd2bin = i_from_f(rd);
	return ri2bin>rd2bin?(ri2bin-rd2bin):(rd2bin-ri2bin);
}


void exhaustiveTest() {
	ofstream os("diff.csv",ofstream::out);
	os << setprecision(100);
	binary32 b;
	int32_t error = 0;
	binary32 sup;
	binary32 inf;
	sup.ui32 = 0b00000000000000000000000000000000 ;
	inf.ui32 = 0b01111111011111111111111111111111 ;
	for (uint32_t i = sup.ui32; i < inf.ui32; i++) {
		b.ui32 = i;
		int32_t k = singleTest(b.f);
		int32_t l = singleTest(-b.f);
		if (k>0) {
			error++;
			cout << "x : " << b.f << " diff : " << k << endl;
			os << b.f << " : " << k << endl;
		}

		if (l > 0) {
			error++;
			cout << "x : " << -b.f << " diff : " << l << endl;
			os << -b.f << " : " << l << endl;
		}
	}
	cout << "Errors : " << error << "/" << 2*(inf.ui32-sup.ui32) << endl;
	os.close();
}

void singleTest2(float x) {
	uint64_t t0,t1,t_empty;
	t0 = get_cycle_counter();
	float x1 = dummy(x);
	t1 = get_cycle_counter();
	t_empty = t1-t0;

	cout << t_empty << endl;

	binary32 b;

	b.f = x;
	cout << "x : " << bitset<32>(b.ui32) << endl;
	t0 = get_cycle_counter();
	float f1 = expf64(x);
	t1 = get_cycle_counter();
	b.f = f1;
	cout << "expf64(x) = ";
	cout << b.f << endl;
	cout << bitset<32>(b.ui32) << endl;
	cout << "Cycle : " << t1-t0-t_empty << endl;
	cout << "----------------------" << endl;

	double f2 = exp((double)x);
	cout << "exp(x)    = ";
	cout << f2 << endl;
	binary64 b64;
	b64.d = f2;
	cout << bitset<64>(b64.ui64) << endl;
	cout << "----------------------" << endl;

	t0 = get_cycle_counter();
	float f3 = expf(x);
	t1 = get_cycle_counter();
	cout << "exp(float)= ";
	binary32 b32;
	b32.f = f3;
	cout << f3 << endl;
	cout << bitset<32>(b32.ui32) << endl;
	cout << "Cycle : " << t1-t0-t_empty << endl;
}

void randTest() {
	srand (time (0));

	unsigned long long i;

	for (i=0; i<RANDTESTSIZE; i++) {
		float x = ((double)((unsigned)rand() | 1) / (double)RAND_MAX) * 160 - 80;
		//		printf("%llu   %1.34e\n", i,x);
		randtest[i]=x;
	}

	uint64_t t0,t1,t_empty;
	cout << randtest[1] << endl;
	t0 = get_cycle_counter();
	for (i = 0; i < RANDTESTSIZE; i++) {
		float x = randtest[i];
		float y = dummy(x);
	}
	t1 = get_cycle_counter();
	t_empty = t1 - t0;
	cout << "Empty loop cycles : " << (double)t_empty/RANDTESTSIZE << endl;
	
	t0 = get_cycle_counter();
	for (i = 0; i < RANDTESTSIZE; i++) {
		float x = randtest[i];
		float y = expf64(x);
	}
	t1 = get_cycle_counter();
	cout << "Expf cycles : " << (double)(t1-t0-t_empty)/RANDTESTSIZE << endl;
	
	t0 = get_cycle_counter();
	for (i = 0; i < RANDTESTSIZE; i++) {
		float x = randtest[i];
		float y = expf(x);
	}
	t1 = get_cycle_counter();
	cout << "Libc expf cycles : " << (double)(t1-t0)/RANDTESTSIZE << endl;

}

int main() {
	cout << setprecision(40);
	//exhaustiveTest();
	//singleTest2(-0.000664663617499173);
	//cout << "-------------------------------------" << endl;
	//singleTest2(5.588646305985673495841936642136715818197e-15);
	randTest();
	return 0;
}