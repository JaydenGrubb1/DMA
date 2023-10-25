#define _USE_MATH_DEFINES
#include <cmath>

#include "fft.h"
#include "onset.h"

namespace DMA::Onset {
	void analyze(std::vector<float>& in, std::vector<float>& out) {
		out.resize(in.size() / (FFT::WINDOW_SIZE / 2));
		float max = 0;

		for (int i = 0; i < out.size(); i++) {
			out[i] = 0;
			for (int j = 0; j < FFT::WINDOW_SIZE / 2; j++) {
				float sample = in[i * (FFT::WINDOW_SIZE / 2) + j];
				out[i] += pow(sample * j, 2);
			}

			if (out[i] > max) {
				max = out[i];
			}
		}

		for (int i = 0; i < out.size(); i++) {
			out[i] = pow(out[i] / max, 2);
		}
	}

	void detect(std::vector<float>& in, std::vector<int>& starts, std::vector<int>& stops) {
		int start_idx = 0;
		int stop_idx = 0;

		constexpr auto THRESHOLD = 0.001;

		for (int i = 0; i < in.size(); i++) {
			if (start_idx > stop_idx) {
				if (in[i] < THRESHOLD) {
					stops.push_back(i);
					stop_idx++;
				}
			}
			else if (start_idx == stop_idx) {
				if (in[i] > THRESHOLD) {
					starts.push_back(i);
					start_idx++;
				}
			}
		}

		if (start_idx > stop_idx) {
			stops.push_back(in.size() - 1);
		}
	}
}