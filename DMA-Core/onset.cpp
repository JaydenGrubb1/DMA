#define _USE_MATH_DEFINES
#include <cmath>

#include "fft.h"
#include "onset.h"

namespace DMA::Onset {
	void detect(std::vector<float>& in, std::vector<float>& out) {
		// TODO: Implement onset detection

		out.resize(in.size() / (FFT::WINDOW_SIZE / 2));
		float max = 0;

		for (int i = 0; i < out.size(); i++) {
			out[i] = 0;
			for (int j = 0; j < FFT::WINDOW_SIZE / 2; j++) {
				out[i] += pow(in[i * (FFT::WINDOW_SIZE / 2) + j] * j, 2);
			}

			if (out[i] > max) {
				max = out[i];
			}
		}

		for (int i = 0; i < out.size(); i++) {
			out[i] /= max;
		}
	}
}