#define _USE_PARALLEL_STFT
#define _USE_ITERATIVE_FFT
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

	void init(void) {
		for (int i = 0; i < WINDOW_SIZE; i++) {
			float angle = 2 * M_PI * i / WINDOW_SIZE;
			_twiddles[i] = std::polar(1.0f, -angle);
		}
	}

	void fft(std::span<complex> in, std::span<complex> out) {
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

		fft(even, even_out);
		fft(odd, odd_out);

		for (int i = 0; i < in.size() / 2; i++) {
			complex t = twiddles[i * WINDOW_SIZE / in.size()] * odd_out[i];
			out[i] = even_out[i] + t;
			out[i + in.size() / 2] = even_out[i] - t;
		}
#endif
	}

	void stft(std::vector<complex>& in, std::vector<complex>& out) {
		int window_count = ceil((double)in.size() / WINDOW_SIZE);
		int total_samples = window_count * WINDOW_SIZE;
		in.resize(total_samples, 0);
		out.resize(total_samples);

#ifdef _USE_PARALLEL_STFT
		int num_threads = std::thread::hardware_concurrency();
		int thread_window_count = ceil((double)window_count / num_threads);
		std::vector<std::thread> threads;
		threads.reserve(num_threads);

		for (int i = 0; i < num_threads; i++) {
			threads.push_back(std::thread([i, thread_window_count, window_count, &in, &out]() {
				for (int j = 0; j < thread_window_count; j++) {
					int window_index = i * thread_window_count + j;
					if (window_index >= window_count) {
						break;
					}

					int start = window_index * WINDOW_SIZE;
					std::span<complex> fft_in(in.data() + start, WINDOW_SIZE);
					std::span<complex> fft_out(out.data() + start, WINDOW_SIZE);

					fft(fft_in, fft_out);
				}}
			));
		}

		for (auto& thread : threads) {
			thread.join();
		}
#else
		for (int i = 0; i < total_samples; i += WINDOW_SIZE) {
			std::span<complex> fft_in(in.data() + i, WINDOW_SIZE);
			std::span<complex> fft_out(out.data() + i, WINDOW_SIZE);

			fft(fft_in, fft_out);
		}
#endif

		// TODO: Normalize the output ???
		// TODO: Multiply by a window function (gausian, hamming, etc) ???
		// TODO: Overlap chunks ???
		// TODO: Return only magnitude for positive frequencies ???
	}
}