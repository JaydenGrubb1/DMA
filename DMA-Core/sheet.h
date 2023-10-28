#pragma once

#include <vector>

#include "note.h"

namespace DMA::Music {
	class Sheet {
	public:
		Sheet() = default;
		Sheet(const char* file);
	private:
		std::vector<Note> _notes;
	};
}