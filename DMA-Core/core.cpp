#include <cstdio>
#include <fstream>
#include <vector>

#include "core.h"
#include "fft.h"
#include "wav.h"

namespace DMA {
	void Test(void) {
		printf("Executing DMA-Core::Test()\n");

		Audio::WAV wav("../../sample.wav");

		auto num_samples = wav.size() / wav.sample_size();
		std::vector<complex> in(num_samples);
		std::vector<complex> out(num_samples);

		for (size_t i = 0; i < num_samples; i += wav.sample_size()) {
			in[i] = complex((float)wav.data()[i], 0.0);
		}

		FFT::init();
		FFT::stft(in, out);

		std::ofstream stream;
		stream.open("../../output.csv");

		std::vector<float> freq(out.size() / 2);
		int chunks = out.size() / FFT::WINDOW_SIZE;
		int chunk_size = FFT::WINDOW_SIZE / 2;

		for (int i = 0; i < chunks; i++) {
			for (int j = 0; j < chunk_size; j++) {
				float magnitude = std::abs(out[i * FFT::WINDOW_SIZE + j]);
				stream << magnitude << ",";
				freq[i * chunk_size + j] = magnitude;
			}
			stream << "\n";
		}

		stream.close();
	}
}