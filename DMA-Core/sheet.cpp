#include <fstream>
#include <stdexcept>

#include <cstdio>

#include "sheet.h"
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;

namespace DMA::Music {
	// TODO: Add wchar version

	Sheet::Sheet(const char* file) {
		XMLDocument doc;
		doc.LoadFile(file);
	}
}