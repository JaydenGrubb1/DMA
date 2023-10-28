#define _USE_MATH_DEFINES
#include <cmath>
#include <stdexcept>

#include "note.h"

namespace DMA::Music {
	Note::Note(float frequency, float duration) : _frequency(frequency), _duration(duration) { }

	Note::Note(char step, int octave, int alter, float duration) : _duration(duration) {
		Pitch pitch = parse(step);

		if (pitch == Invalid) {
			throw std::runtime_error("Invalid step");
		}

		pitch = (Pitch)(alter + (int)pitch);
		_frequency = 440 * pow(2, (pitch - A) / 12.0f + (octave - 4));
	}

	Pitch Note::parse(char c) {
		switch (c) {
		case 'C':
			return C;
		case 'D':
			return D;
		case 'E':
			return E;
		case 'F':
			return F;
		case 'G':
			return G;
		case 'A':
			return A;
		case 'B':
			return B;
		default:
			return Invalid;
		}
	}
}