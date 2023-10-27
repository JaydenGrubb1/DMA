#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

#include "fft.h"
#include "onset.h"
#include "sheet.h"
#include "wav.h"

using namespace DMA;

constexpr auto ITERATIONS = 1000;

int main(int argc, char** argv) {
	Audio::WAV wav(L"../../sample.wav");
	Music::Sheet sheet("../../sample.xml");
	
	std::vector<complex> in(wav.num_samples());
	std::vector<complex> out;

	for (size_t i = 0; i < wav.num_samples(); i++) {
		in[i] = complex(wav[i], 0);
	}

	FFT::stft(in, out);

	std::vector<float> freq;
	FFT::format(out, freq);

	std::vector<float> hfc;
	Onset::analyze(freq, hfc);

	std::vector<int> starts;
	std::vector<int> stops;
	Onset::detect(hfc, starts, stops);

	std::vector<float> notes;
	Onset::identify(in, starts, stops, wav.sample_rate(), notes);

	return EXIT_SUCCESS;
}