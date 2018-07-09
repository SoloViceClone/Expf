using namespace std;

#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <bitset>

extern float expf64(float x);

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
	float rd = exp(x);
	int32_t ri2bin = i_from_f(ri);
	int32_t rd2bin = i_from_f(rd);
	return ri2bin>rd2bin?(ri2bin-rd2bin):(rd2bin-ri2bin);
}

void exhaustiveTest() {
	ofstream os("worst_case.txt",ofstream::out);
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
			os << b.f << endl;
		}

		if (l > 0) {
			error++;
			cout << "x : " << -b.f << " diff : " << l << endl;
			os << -b.f << endl;
		}
	}
	cout << "Errors : " << error << "/" << 2*(inf.ui32-sup.ui32) << endl;
	os.close();
}

void singleTest2(float x) {
	binary32 b;

	b.f = x;
	cout << "x : " << bitset<32>(b.ui32) << endl;
	uint64_t t0 = get_cycle_counter();
	b.f = expf64(x);
	uint64_t t1 = get_cycle_counter();
	cout << "expf64(x) = ";
	cout << b.f << endl;
	cout << bitset<32>(b.ui32) << endl;
	cout << "Cycle : " << t1-t0 << endl;
	cout << "----------------------" << endl;

	double f2 = exp((double)x);
	cout << "exp(x)    = ";
	cout << f2 << endl;
	binary64 b64;
	b64.d = f2;
	cout << bitset<64>(b64.ui64) << endl;
	cout << "----------------------" << endl;

	t0 = get_cycle_counter();
	float f3 = exp(x);
	t1 = get_cycle_counter();
	cout << "exp(float)= ";
	binary32 b32;
	b32.f = f3;
	cout << f3 << endl;
	cout << bitset<32>(b32.ui32) << endl;
	cout << "Cycle : " << t1-t0 << endl;
}

void forfun() {
	int64_t e = -140;
	//int64_t result = (36028797018963967 + (((e + 126) >> 63) << 55));
	int64_t result = (-127-e) & ((e+126) >> 63);
	cout << result << endl;
	cout << bitset<64>(result) << endl;
}

int main() {
	cout << setprecision(100);
	//exhaustiveTest();
	singleTest2(88.72017669677734375);
	//forfun();
	return 0;
}