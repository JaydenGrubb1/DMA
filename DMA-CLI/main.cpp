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

int main(int argc, char** argv) {
	auto start_time = std::chrono::high_resolution_clock::now();

	Audio::WAV wav;
	Music::Sheet sheet;

	try {
		wav = Audio::WAV(L"../data/sample.wav");
		sheet = Music::Sheet("../data/sample.xml");
	}
	catch (std::exception& e) {
		printf("Error: %s\n", e.what());
		return EXIT_FAILURE;
	}

	auto file_load_time = std::chrono::high_resolution_clock::now();

	std::vector<complex> in(wav.num_samples());
	std::vector<complex> out;

	for (size_t i = 0; i < wav.num_samples(); i++) {
		in[i] = complex(wav[i], 0);
	}

	FFT::init();
	FFT::stft(in, out);

	auto fft_time = std::chrono::high_resolution_clock::now();

	std::vector<float> freq;
	FFT::format(out, freq);

	std::vector<float> hfc;
	Onset::analyze(freq, hfc);

	std::vector<int> starts;
	std::vector<int> stops;
	Onset::detect(hfc, starts, stops);

	std::vector<float> notes;
	Onset::identify(in, starts, stops, wav.sample_rate(), notes);

	auto onset_time = std::chrono::high_resolution_clock::now();

	auto file_load_duration = std::chrono::duration_cast<std::chrono::milliseconds>(file_load_time - start_time);
	auto fft_duration = std::chrono::duration_cast<std::chrono::milliseconds>(fft_time - file_load_time);
	auto onset_duration = std::chrono::duration_cast<std::chrono::milliseconds>(onset_time - fft_time);
	auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(onset_time - start_time);

	printf("File load time: %5zu ms\n", file_load_duration.count());
	printf("FFT time:       %5zu ms\n", fft_duration.count());
	printf("Onset time:     %5zu ms\n", onset_duration.count());
	printf("Total time:     %5zu ms\n", total_duration.count());
	printf("\n");

	//// BELOW CODE VERY HACKY /// NO TIME TO UNDERSTAND MUSIC STUFF ///

	Music::Sheet actual;

	for (int i = 0; i < notes.size(); i++) {
		actual.add(notes[i], 1);
	}

	printf("Note | Expected | Actual |\n");
	printf("-----+----------+--------|\n");
	for (int i = 0; i < std::max(sheet.size(), actual.size()); i++) {
		std::string actual_note = "";
		if (i < actual.size()) {
			actual_note = actual[i].to_string();
		}

		std::string sheet_note = "";
		if (i < sheet.size()) {
			sheet_note = sheet[i].to_string();
		}

		printf("%4d | %8s | %6s |\n", i, sheet_note.c_str(), actual_note.c_str());
	}

	return EXIT_SUCCESS;
}