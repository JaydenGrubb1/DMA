#define _USE_MATH_DEFINES
#include <cmath>
#include <stdexcept>

#include "note.h"

namespace DMA::Music {
	Note::Note(float frequency, float duration) : _frequency(frequency), _duration(duration) { }

	Note::Note(Pitch pitch, int octave, float duration) : _duration(duration) {
		if (pitch == Invalid) {
			throw std::runtime_error("Invalid pitch");
		}

		_frequency = 440 * pow(2, (pitch - A) / 12.0f + (octave - 4));
	}

	Note::Note(char step, int octave, int alter, float duration) : _duration(duration) {
		Pitch pitch = parse(step, alter);

		if (pitch == Invalid) {
			throw std::runtime_error("Invalid step");
		}

		_frequency = 440 * pow(2, (pitch - A) / 12.0f + (octave - 4));
	}

	Pitch Note::pitch() const {
		float pitch = 12 * log2(_frequency / 440) + 69;
		return (Pitch)((int)round(pitch) % 12);
	}

	int Note::octave() const {
		float pitch = 12 * log2(_frequency / 440) + 69;
		return (int)round(pitch) / 12 - 1;
	}

	std::string Note::to_string() const {
		std::string str = "";

		switch (pitch()) {
		case C:
			str += "C";
			break;
		case Db:
			str += "Db";
			break;
		case D:
			str += "D";
			break;
		case Eb:
			str += "Eb";
			break;
		case E:
			str += "E";
			break;
		case F:
			str += "F";
			break;
		case Gb:
			str += "Gb";
			break;
		case G:
			str += "G";
			break;
		case Ab:
			str += "Ab";
			break;
		case A:
			str += "A";
			break;
		case Bb:
			str += "Bb";
			break;
		case B:
			str += "B";
			break;
		}

		if (pitch() > Invalid) {
			str += std::to_string(octave());
		}

		return str;
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

	Pitch Note::parse(char c, int alter) {
		Pitch pitch = parse(c);
		if (pitch == Invalid) {
			return Invalid;
		}
		return (Pitch)(alter + (int)pitch);
	}
}