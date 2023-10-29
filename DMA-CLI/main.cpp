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
	Audio::WAV wav(L"../../sample.wav");
	Music::Sheet sheet("../../sample.xml");

	std::vector<complex> in(wav.num_samples());
	std::vector<complex> out;

	for (size_t i = 0; i < wav.num_samples(); i++) {
		in[i] = complex(wav[i], 0);
	}

	FFT::init();
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

	//// BELOW CODE VERY HACKY /// NO TIME TO UNDERSTAND MUSIC STUFF ///

	Music::Sheet actual;
	actual.notes().reserve(notes.size());

	for (int i = 0; i < notes.size(); i++) {
		// TODO: Properly calculate note duration
		actual.notes().emplace_back(notes[i], 1);
	}

	printf("Note | Expected | Actual |\n");
	printf("-----+----------+--------|\n");
	for (int i = 0; i < std::max(sheet.size(), actual.size()); i++)
	{
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