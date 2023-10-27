#pragma once

#include <vector>

namespace DMA::Music {
	struct Note {
		int pitch;
		float duration;

		Note() = default;
		Note(int pitch, float duration) : pitch(pitch), duration(duration) {}
		Note(char step, int octave, int alter, int duration) {

		}
	};

	class Sheet {
	public:
		Sheet() = default;
		Sheet(const char* file);
	private:
		std::vector<Note> _notes;
	};
}