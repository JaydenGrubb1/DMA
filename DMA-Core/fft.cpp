#define _USE_PARALLEL_STFT
#define _USE_ITERATIVE_FFT

#ifndef _STFT_PARALLEL_SCALE
#define _STFT_PARALLEL_SCALE 2
#endif

#define _USE_MATH_DEFINES
#include <cmath>
#include <thread>

#include "fft.h"

namespace DMA::FFT {
	static complex _twiddles[WINDOW_SIZE];

#ifdef _USE_ITERATIVE_FFT
	static int __bit_reverse(int x, int log2n) {
		int n = 0;
		for (int i = 0; i < log2n; i++) {
			n <<= 1;
			n |= (x & 1);
			x >>= 1;
		}
		return n;
	}
#endif

	static void __fft(std::span<complex> in, std::span<complex> out) {
#ifdef _USE_ITERATIVE_FFT
		int log2n = log2(in.size());

		for (int i = 0; i < in.size(); i++) {
			int rev = __bit_reverse(i, log2n);
			out[i] = in[rev];
		}

		for (int i = 1; i <= log2n; i++) {
			int size = 1 << i;	// 2^i

			for (int j = 0; j < size / 2; j++) {
				for (int k = j; k < in.size(); k += size) {
					complex w = _twiddles[j * WINDOW_SIZE / size];
					complex t = out[k + size / 2] * w;
					complex u = out[k];

					out[k] = u + t;
					out[k + size / 2] = u - t;
				}
			}
		}
#else
		if (in.size() == 1) {
			out[0] = in[0];
			return;
		}

		std::vector<complex> even(in.size() / 2);
		std::vector<complex> odd(in.size() / 2);
		std::vector<complex> even_out(in.size() / 2);
		std::vector<complex> odd_out(in.size() / 2);

		for (int i = 0; i < in.size() / 2; i++) {
			even[i] = in[2 * i];
			odd[i] = in[2 * i + 1];
		}

		__fft(even, even_out);
		__fft(odd, odd_out);

		for (int i = 0; i < in.size() / 2; i++) {
			complex t = _twiddles[i * WINDOW_SIZE / in.size()] * odd_out[i];
			out[i] = even_out[i] + t;
			out[i + in.size() / 2] = even_out[i] - t;
		}
#endif
	}

	void init(void) {
		for (int i = 0; i < WINDOW_SIZE; i++) {
			float angle = 2 * M_PI * i / WINDOW_SIZE;
			_twiddles[i] = std::polar(1.0f, -angle);
		}
	}

	void fft(std::span<complex> in, std::vector<complex>& out) {
		int size = 1 << (int)ceil(log2(in.size()));
		std::vector<complex> temp(size, 0);
		out.resize(size);
		memcpy(temp.data(), in.data(), in.size() * sizeof(complex));
		__fft(temp, out);
	}

	void stft(std::vector<complex>& in, std::vector<complex>& out) {
		int window_count = floor((double)in.size() / WINDOW_SIZE);
		int total_samples = window_count * WINDOW_SIZE;
		in.resize(total_samples, 0);
		out.resize(total_samples * WINDOW_OVERLAP);

#ifdef _USE_PARALLEL_STFT
		int num_threads = std::thread::hardware_concurrency() / _STFT_PARALLEL_SCALE;
		int thread_samples = ceil((double)window_count / num_threads) * WINDOW_SIZE;
		std::vector<std::thread> threads;
		threads.reserve(num_threads);

		for (int i = 0; i < num_threads; i++) {
			int start = i * thread_samples;
			int end = std::min(start + thread_samples, total_samples);
			threads.emplace_back([&, start, end]() {
				for (int i = start; i < end; i += WINDOW_SIZE / WINDOW_OVERLAP) {
					if (i + WINDOW_SIZE > total_samples) {
						break;
					}
					std::span<complex> fft_in(in.data() + i, WINDOW_SIZE);
					std::span<complex> fft_out(out.data() + i * WINDOW_OVERLAP, WINDOW_SIZE);
					__fft(fft_in, fft_out);
				}}
			);
		}

		for (auto& thread : threads) {
			thread.join();
		}
#else
		for (int i = 0; i < total_samples; i += WINDOW_SIZE / WINDOW_OVERLAP) {
			std::span<complex> fft_in(in.data() + i, WINDOW_SIZE);
			std::span<complex> fft_out(out.data() + i * WINDOW_OVERLAP, WINDOW_SIZE);

			__fft(fft_in, fft_out);
		}
#endif
	}

	void format(std::vector<complex>& in, std::vector<float>& out) {
		out.resize(in.size() / 2);
		float max = 0.0f;

		for (int i = 0; i < in.size() / FFT::WINDOW_SIZE; i++) {
			for (int j = 1; j < FFT::WINDOW_SIZE / 2; j++) {
				float magnitude = std::abs(in[i * FFT::WINDOW_SIZE + j]);
				out[i * FFT::WINDOW_SIZE / 2 + j] = magnitude;

				if (magnitude > max) {
					max = magnitude;
				}
			}
		}

		for (int i = 0; i < in.size() / 2; i++) {
			out[i] /= max;
		}

		// TODO: Parallelize this function ???
	}
}