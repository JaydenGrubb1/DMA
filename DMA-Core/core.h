#pragma once

#ifdef DMACORE_EXPORTS
#define DMACORE_API __declspec(dllexport)
#else
#define DMACORE_API __declspec(dllimport)
#endif

extern "C" DMACORE_API int Test(int a, int b);