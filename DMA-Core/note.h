#pragma once

namespace DMA::Music {
	enum Pitch {
		Invalid = -1,
		C = 0,
		Db = 1,
		D = 2,
		Eb = 3,
		E = 4,
		F = 5,
		Gb = 6,
		G = 7,
		Ab = 8,
		A = 9,
		Bb = 10,
		B = 11
	};

	class Note {
	public:
		Note() = default;
		Note(int pitch, float duration) : _pitch(pitch), _duration(duration) {}
		Note(char step, int octave, int alter, int duration) {

		}
	private:
		int _pitch = 0;
		float _duration = 0;
	};
}