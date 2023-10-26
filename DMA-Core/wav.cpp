#include <fstream>
#include <stdexcept>

#include "wav.h"

namespace DMA::Audio {
	WAV::WAV(const wchar_t* file) {
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

	WAV& WAV::operator=(WAV&& other) noexcept {
		if (this != &other) {
			delete[] _data;
			_header = other._header;
			_data = other._data;
			other._data = nullptr;
		}
		return *this;
	}

	float WAV::operator [](const size_t& index) const {
		size_t offset = index * _header.sample_size / 8;

		if (_header.sample_size == 8) {
			uint8_t sample = _data[offset];
			return ((float)sample - 128) / 128;
		}
		else if (_header.sample_size == 16) {
			uint16_t sample = _data[offset + 1] << 8 | _data[offset];
			return ((float)sample - 128) / 128;
		}
		else if (_header.sample_size == 24) {
			uint32_t sample = _data[offset + 2] << 16 | _data[offset + 1] << 8 | _data[offset];
			return ((float)sample - 128) / 128;
		}
		else if (_header.sample_size == 32) {
			uint32_t sample = _data[offset + 3] << 24 | _data[offset + 2] << 16 | _data[offset + 1] << 8 | _data[offset];
			return ((float)sample - 128) / 128;
		}
	}

	WAV::~WAV() {
		delete[] _data;
	}
}