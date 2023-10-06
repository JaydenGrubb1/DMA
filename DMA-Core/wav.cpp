#include <stdexcept>

#include <cstdio>

#include "wav.h"

namespace DMA::Audio {
	WAV::WAV(const char* file) {
		if (fopen_s(&fp, file, "rb")) {
			throw std::runtime_error("Could not open file");
		}

		// TODO: Implement
	}

	WAV::~WAV() {
		// TODO: Implement
	}
}