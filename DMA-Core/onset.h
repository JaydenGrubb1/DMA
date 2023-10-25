#pragma once

#include <vector>

namespace DMA::Onset {
	void analyze(std::vector<float>& in, std::vector<float>& out);
	void detect(std::vector<float>& in, std::vector<int>& starts, std::vector<int>& stops);
}