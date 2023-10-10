#define _USE_SIMD_COMPLEX
#include <cmath>
#include <immintrin.h>

#include "comp.h"

namespace DMA {
	complex operator+(const complex& a, const complex& b) {
#ifdef _USE_SIMD_COMPLEX
		complex res;
		__m128d num1, num2;
		num1 = _mm_loaddup_pd(&a.real);
		num2 = _mm_loaddup_pd(&b.real);
		num1 = _mm_add_pd(num1, num2);
		_mm_store_pd(&res.real, num1);
		return res;
#else
		return { a.real + b.real, a.imag + b.imag };
#endif
	}

	complex operator-(const complex& a, const complex& b) {
#ifdef _USE_SIMD_COMPLEX
		complex res;
		__m128d num1, num2;
		num1 = _mm_loaddup_pd(&a.real);
		num2 = _mm_loaddup_pd(&b.real);
		num1 = _mm_sub_pd(num1, num2);
		_mm_store_pd(&res.real, num1);
		return res;
#else
		return { a.real - b.real, a.imag - b.imag };
#endif
	}

	complex operator*(const complex& a, const complex& b) {
#ifdef _USE_SIMD_COMPLEX
		complex res;
		__m128d num1, num2, num3;
		num1 = _mm_loaddup_pd(&a.real);
		num2 = _mm_set_pd(b.imag, b.real);
		num3 = _mm_mul_pd(num2, num1);
		num1 = _mm_loaddup_pd(&a.imag);
		num2 = _mm_shuffle_pd(num2, num2, 1);
		num2 = _mm_mul_pd(num2, num1);
		num3 = _mm_addsub_pd(num3, num2);
		_mm_store_pd(&res.real, num3);
		return res;
#else
		return { a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real };
#endif
	}
}

namespace std {
	double abs(const DMA::complex& c) {
		return sqrt(c.real * c.real + c.imag * c.imag);
	}
}