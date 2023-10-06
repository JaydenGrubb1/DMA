#pragma once

#include <complex>
#include <span>

#include "conf.h"

using complex = std::complex<float>;

namespace DMA::FFT {
	/// <summary>
	/// Precomputed twiddle factors
	/// </summary>
	static complex twiddles[TWIDDLE_SIZE];

	/// <summary>
	/// Initializes the FFT module
	/// </summary>
	void init(void);

	/// <summary>
	/// Computes the FFT of the input buffer and stores the result in the output buffer
	/// </summary>
	/// <param name="in">The input buffer</param>
	/// <param name="out">The output buffer</param>
	void fft(std::span<complex> in, std::span<complex> out);
}