#pragma once

#include <string>

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
		/// <summary>
		/// Constructs an empty note
		/// </summary>
		Note() = default;

		/// <summary>
		/// Constructs a note with the given frequency and duration
		/// </summary>
		/// <param name="frequency">The frequency of the note</param>
		/// <param name="duration">The duration of the note</param>
		Note(float frequency, float duration);

		/// <summary>
		/// Constructs a note with the given pitch, octave and duration
		/// </summary>
		/// <param name="pitch">The pitch of the note</param>
		/// <param name="octave">The octave of the note</param>
		/// <param name="duration">The duration of the note</param>
		Note(Pitch pitch, int octave, float duration);

		/// <summary>
		/// Constructs a note with the given step, octave, alter and duration
		/// </summary>
		/// <param name="step">The step of the note</param>
		/// <param name="octave">The octave of the note</param>
		/// <param name="alter">The alter of the note</param>
		/// <param name="duration">The duration of the note</param>
		Note(char step, int octave, int alter, float duration);

		/// <summary>
		/// Returns the frequency of the note
		/// </summary>
		/// <returns>The frequency of the note</returns>
		float frequency() const { return _frequency; }

		/// <summary>
		/// Returns the duration of the note
		/// </summary>
		/// <returns>The duration of the note</returns>
		float duration() const { return _duration; }

		/// <summary>
		/// Returns the pitch of the note
		/// </summary>
		/// <returns>The pitch of the note</returns>
		Pitch pitch() const;

		/// <summary>
		/// Returns the octave of the note
		/// </summary>
		/// <returns>The octave of the note</returns>
		int octave() const;

		/// <summary>
		/// Returns the string representation of the note
		/// </summary>
		/// <returns>The string representation of the note</returns>
		std::string to_string() const;

		/// <summary>
		/// Parses the given character into a pitch
		/// </summary>
		/// <param name="c">The character to parse</param>
		/// <returns>The parsed pitch</returns>
		static Pitch parse(char c);

		/// <summary>
		/// Parses the given character and alter into a pitch
		/// </summary>
		/// <param name="c">The character to parse</param>
		/// <param name="alter">The alter to parse</param>
		/// <returns>The parsed pitch</returns>
		static Pitch parse(char c, int alter);
	private:
		float _frequency = 0;
		float _duration = 0;
	};
}