#pragma once

#include <array>
#include <complex>
#include <span>

#define TWIDDLE_SIZE 2048

using complex = std::complex<float>;

namespace DMA::FFT {

	static std::array<complex, TWIDDLE_SIZE> twiddles;

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