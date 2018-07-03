using namespace std;

#include <math.h>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <bitset>
#include <iomanip>
#include <float.h>

#define TEST_MIN -0x1.5d58ap6

#define TEST_MAX 0x1.62e43p6

#define VALID_RANGE TEST_MAX-TEST_MIN

#define RANDTESTSIZE 100000

extern float expf64(float x);

typedef union {
	float f;
	int32_t i32;
	uint32_t ui32;
} binary32;

int32_t i_from_f(float x) {
	binary32 b;
	b.f = x;
	return b.ui32;
}

float test(float x) {
	float ri = expf64(x);
	float rd = exp(x);
	int32_t ri2bin = i_from_f(ri);
	int32_t rd2bin = i_from_f(rd);
	int32_t x2bin = i_from_f(x);
#ifdef DEBUG
	
	cout << setprecision(100);
	cout << "Testing : exp(" << x << ")" << endl;
	cout << "    x     : " << bitset<32>(x2bin)  << "   " << x  << endl;
	cout << "    Fixed : " << bitset<32>(ri2bin) << "   " << ri << endl;
	cout << "    libc  : " << bitset<32>(rd2bin) << "   " << rd << endl;
#endif
	if (ri2bin == rd2bin) {
		return 0;
	} else {
		cout << "Testing : exp(" << x << ")" << endl;
		cout << "    x     : " << bitset<32>(x2bin)  << "   " << x  << endl;
		cout << "    Fixed : " << bitset<32>(ri2bin) << "   " << ri << endl;
		cout << "    libc  : " << bitset<32>(rd2bin) << "   " << rd << endl;
		return (ri>rd)?(ri-rd):(rd-ri);
		
	}
}

int testRange(float range) {
	int nbTrue = 0;
	float error = 0;
	for (int i = 0; i < RANDTESTSIZE; i++) {
		int r = rand();
		float x  = ((float)r / (float)RAND_MAX) + (float)range ;
		float er = test(x);
		error += er;
		if (er == 0) {
			nbTrue++;
#ifdef DEBUG
			cout << "TRUE" << endl;
#endif
		} else {
#ifdef DEBUG
			cout << "FALSE" << endl;
#endif
		}
#ifdef DEBUG
		cout << "------------------" << endl;
#endif
 	}
#ifdef DEBUG
	cout << "-------------------------------------" << endl;
	cout << "Test OK : " << nbTrue << "/" << RANDTESTSIZE << endl;
#endif
	cout << "TestRange " << range << " " << nbTrue << " mean_error " << error/RANDTESTSIZE << endl;
	return nbTrue;
	
}

void test1() {
	int total = 0;
	int low = -85;
	int hi = 90;
	for (int i = low; i < hi; i++) {
		total += testRange((float)i);
	}

	cout << "--------Total : " << (hi-low)*RANDTESTSIZE-total << "/" << (hi-low)*RANDTESTSIZE << endl;
}

void test2() {
	float k = 0.0000000000000001;
	cout << k << endl;
	cout << expf64(k) << endl;
	cout << exp(k) << endl;
	test(k);
}

void test3() {
	float x = -31.505603790283203125;
	cout << x << endl;
	cout << exp(x) << endl;
	cout << "---------------" << endl;
	test(x);
}

int32_t singleTest(float x) {
	float ri = expf64(x);
	float rd = exp(x);
	int32_t ri2bin = i_from_f(ri);
	int32_t rd2bin = i_from_f(rd);
	return ri2bin>rd2bin?(ri2bin-rd2bin):(rd2bin-ri2bin);
}

/*
	Exhaustive test for floating point number
*/
void test4() {
	binary32 b;
	int32_t error = 0;
	binary32 sup;
	binary32 inf;
	sup.ui32 = 0b00000000100000000000000000000000;
	inf.f = 100;
	for (uint32_t i = sup.ui32; i < inf.ui32; i++) {
		b.ui32 = i;
		int32_t k = singleTest(b.f);
		
		if (k>0) {
			error++;
			cout << "x : " << b.f << " diff : " << k << endl;
		}
	}
	cout << "Errors : " << error << "/" << inf.ui32-sup.ui32 << endl;
}

void test4fun() {
	binary32 b;
	b.f = 1;
	cout << bitset<32>(b.ui32) << endl;
	cout << b.f << endl;
}

void test5() {
	binary32 b;
	b.f = 5.06926562889690940034189988158662931616474205120947971181522007100284099578857421875e-27;
	//cout << singleTest(b.f) << endl;
	b.f = expf64(b.f);
	cout << bitset<32>(b.ui32) << endl;
	cout << b.f << endl;
}

/*
	Get some useful values
*/
void test6() {

	cout << FLT_MAX << endl;
}
int main() {

	srand (time (0));
	cout << setprecision(100);
	test4();
}