#pragma once

#ifdef DMACORE_EXPORTS
#define DMACORE_API __declspec(dllexport)
#else
#define DMACORE_API __declspec(dllimport)
#endif

namespace DMA {
	extern "C" DMACORE_API void Test(void);
}