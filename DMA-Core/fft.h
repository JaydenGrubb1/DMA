#pragma once

#include <complex>
#include <span>
#include <vector>

using complex = std::complex<float>;

namespace DMA::FFT {
	/// <summary>
	/// The size of the twiddle factor table
	/// </summary>
	constexpr auto WINDOW_SIZE = 2048;

	/// <summary>
	/// Precomputed twiddle factors
	/// </summary>
	static complex twiddles[WINDOW_SIZE];

	/// <summary>
	/// Initializes the FFT module
	/// </summary>
	void init(void);

	/// <summary>
	/// Computes the FFT (Fast Fourier Transform) of the given data
	/// </summary>
	/// <param name="in">The input buffer</param>
	/// <param name="out">The output buffer</param>
	void fft(std::span<complex> in, std::span<complex> out);

	/// <summary>
	///	Computes the STFT (Short Time Fourier Transform) of the give data
	/// </summary>
	/// <param name="in">The input buffer</param>
	/// <param name="out">The output buffer</param>
	void stft(std::vector<complex>& in, std::vector<complex>& out);
}