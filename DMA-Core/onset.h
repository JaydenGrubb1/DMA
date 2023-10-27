#pragma once

#include <span>
#include <vector>

namespace DMA::Onset {
	/// <summary>
	/// The threshold for the onset detection to consider a new onset
	/// </summary>
	constexpr auto ONSET_THRESHOLD = 0.0015;

	/// <summary>
	/// The threshold for the harmonic ratio to consider a frequency as a harmonic
	/// </summary>
	constexpr auto HARMONIC_RATIO_THRESHOLD = 0.1;

	/// <summary>
	/// Analyzes the given data using the HFC (High Frequency Content) algorithm
	/// </summary>
	/// <param name="in">The input buffer</param>
	/// <param name="out">The output buffer</param>
	void analyze(std::vector<float>& in, std::vector<float>& out);

	/// <summary>
	/// Detects the onsets and offsets of the given data
	/// </summary>
	/// <param name="in">The input buffer</param>
	/// <param name="starts">The output buffer for the onsets</param>
	/// <param name="stops">The output buffer for the offsets</param>
	void detect(std::vector<float>& in, std::vector<int>& starts, std::vector<int>& stops);

	/// <summary>
	/// Identifies the frequencies contained in the given data between the given onsets and offsets
	/// </summary>
	/// <param name="in">The signal input buffer</param>
	/// <param name="starts">The onset input buffer</param>
	/// <param name="stops">The offset input buffer</param>
	/// <param name="sampling_rate">The sampling rate of the signal</param>
	/// <param name="out">The frequency output buffer</param>
	void identify(std::span<complex> in, std::span<int> starts, std::span<int> stops, float sampling_rate, std::vector<float>& out);
}