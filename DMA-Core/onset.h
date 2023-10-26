#pragma once

#include <vector>

namespace DMA::Onset {
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
}