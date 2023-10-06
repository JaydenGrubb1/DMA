#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

#include "fft.h"

namespace DMA::FFT {
	void init(void) {
		for (int i = 0; i < TWIDDLE_SIZE; i++) {
			float angle = 2 * M_PI * i / TWIDDLE_SIZE;
			twiddles[i] = std::polar(1.0f, -angle);
		}
	}

	void fft(std::span<complex> in, std::span<complex> out)
	{
		if (in.size() == 1)
		{
			out[0] = in[0];
			return;
		}

		std::vector<complex> even(in.size() / 2);
		std::vector<complex> odd(in.size() / 2);
		std::vector<complex> even_out(in.size() / 2);
		std::vector<complex> odd_out(in.size() / 2);

		for (int i = 0; i < in.size() / 2; i++)
		{
			even[i] = in[2 * i];
			odd[i] = in[2 * i + 1];
		}

		fft(even, even_out);
		fft(odd, odd_out);

		for (int i = 0; i < in.size() / 2; i++)
		{
			complex t = twiddles[i * TWIDDLE_SIZE / in.size()] * odd_out[i];
			out[i] = even_out[i] + t;
			out[i + in.size() / 2] = even_out[i] - t;
		}
	}
}