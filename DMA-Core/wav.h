#pragma once

namespace DMA::Audio {
	class WAV {
	public:
		WAV(const char* file);
		~WAV();
	private:
		FILE* fp;
	};
}