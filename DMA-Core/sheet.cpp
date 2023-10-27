#include <fstream>
#include <stdexcept>

#include <cstdio>

#include "sheet.h"

namespace DMA::Music {
	Sheet::Sheet(const wchar_t* file) {
		std::ifstream stream;
		stream.open(file);

		if (!stream.is_open()) {
			throw std::runtime_error("Failed to open file");
		}

		// TODO: Is this how I should load an XML, look into tinyxml2
		std::string str((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		printf("%s\n", str.c_str());
	}
}