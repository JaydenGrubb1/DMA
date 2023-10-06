#include <cstdio>
#include <vector>

#include "core.h"
#include "fft.h"
#include "wav.h"

namespace DMA {
	void Test(void) {
		printf("Executing DMA-Core::Test()\n");

		Audio::WAV wav("../../sample.wav");

		std::vector<complex> in = { 4, 7, 2, 3 };
		std::vector<complex> out(in.size());

		FFT::init();
		FFT::stft(in, out);

		printf("\nInput:\n");
		for (auto c : in) {
			printf("% .2f + % 2.2fi\n", c.real(), c.imag());
		}

		printf("\nOutput:\n");
		for (auto c : out) {
			printf("% .2f + % 2.2fi\n", c.real(), c.imag());
		}
	}
}