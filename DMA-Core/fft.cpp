#define _USE_PARALLEL_STFT
#define _USE_MATH_DEFINES
#include <cmath>
#include <thread>

#include "fft.h"

namespace DMA::FFT {
	void init(void) {
		for (int i = 0; i < WINDOW_SIZE; i++) {
			float angle = 2 * M_PI * i / WINDOW_SIZE;
			twiddles[i] = std::polar(1.0f, -angle);
		}
	}

	void fft(std::span<complex> in, std::span<complex> out) {
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
	}

	void stft(std::vector<complex>& in, std::vector<complex>& out) {
		int window_count = ceil((double)in.size() / WINDOW_SIZE);
		int total_samples = window_count * WINDOW_SIZE;
		in.resize(total_samples, 0);
		out.resize(total_samples);

#ifdef _USE_PARALLEL_STFT
		std::vector<std::thread> threads;
		int num_threads = std::thread::hardware_concurrency();
		int thread_window_count = ceil((double)window_count / num_threads);

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