#pragma once

#include <vector>

namespace DMA::Music {
	struct Note {
		int pitch;
		float duration;
	};

	class Sheet {
	public:
		Sheet() = default;
		Sheet(const wchar_t* file);
	private:
		std::vector<Note> _notes;
	};
}