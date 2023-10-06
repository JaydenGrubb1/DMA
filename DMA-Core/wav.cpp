#include <fstream>
#include <stdexcept>

#include "wav.h"

namespace DMA::Audio {
	WAV::WAV(const char* file) {
		std::ifstream stream;
		stream.open(file, std::ios::binary);

		if (!stream.is_open()) {
			throw std::runtime_error("Failed to open file");
		}

		// TODO: Check if file is valid WAV file

		stream.read((char*)&_header, sizeof(_header));
		_data = new char[_header.data_chunk_size];
		stream.read(_data, _header.data_chunk_size);

		// TODO: Normalize audio data ???

		stream.close();
	}

	WAV::~WAV() {
		delete[] _data;
	}
}