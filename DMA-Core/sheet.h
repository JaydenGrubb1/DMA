#pragma once

#include <vector>

#include "note.h"

namespace DMA::Music {
	class Sheet {
	public:
		/// <summary>
		/// Constructs an empty sheet
		/// </summary>
		Sheet() = default;

		/// <summary>
		/// Constructs a sheet from the given file
		/// </summary>
		/// <param name="file">The file to read from</param>
		Sheet(const char* file);

		/// <summary>
		/// Returns the number of notes in the sheet
		/// </summary>
		/// <returns>The number of notes in the sheet</returns>
		size_t size() const { return _notes.size(); }

		/// <summary>
		/// Returns the note at the given index
		/// </summary>
		/// <param name="index">The index of the note</param>
		/// <returns>The note at the given index</returns>
		Note operator[](size_t index) const { return _notes[index]; }
	private:
		std::vector<Note> _notes;
	};
}