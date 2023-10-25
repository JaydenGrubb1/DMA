#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

#include "fft.h"
#include "wav.h"

using namespace DMA;

constexpr auto ITERATIONS = 1000;

int main(int argc, char** argv) {
	Audio::WAV wav(L"../../sample.wav");

	auto num_samples = wav.size() / wav.sample_size();
	std::vector<complex> in(num_samples);
	std::vector<complex> out(num_samples);

	for (size_t i = 0; i < num_samples; i += wav.sample_size()) {
		uint8_t sample = wav.data()[i];
		in[i] = complex((float)sample, 0.0);
	}

	FFT::init();

	auto average = 0;
	for (int i = 0; i < ITERATIONS; i++) {
		auto start = std::chrono::high_resolution_clock::now();
		FFT::stft(in, out);
		auto end = std::chrono::high_resolution_clock::now();
		average += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}
	printf("Average time: %d microseconds\n", average / ITERATIONS);

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

	return EXIT_SUCCESS;
}