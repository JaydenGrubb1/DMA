#include <cstdio>
#include <array>

#include "core.h"
#include "fft.h"

void Test(void)
{
	printf("Executing DMA-Core::Test()\n");

	std::array<complex, 8> in = { 4, 7, 2, 3, 0, 3, 2, 1 };
	std::array<complex, 8> out;

	DMA::FFT::init();
	DMA::FFT::fft(in, out);

	printf("\nInput:\n");
	for (auto c : in)
	{
		printf("% .2f + % 2.2fi\n", c.real(), c.imag());
	}

	printf("\nOutput:\n");
	for (auto c : out)
	{
		printf("% .2f + % 2.2fi\n", c.real(), c.imag());
	}
}