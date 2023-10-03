#pragma once

#ifdef DMACORE_EXPORTS
#define DMACORE_API __declspec(dllexport)
#else
#define DMACORE_API __declspec(dllimport)
#endif

struct Complex {
	double real;
	double imag;
};

extern "C" DMACORE_API void Test(int a, int b, Complex * c);