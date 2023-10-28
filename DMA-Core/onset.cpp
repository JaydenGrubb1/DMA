#define _USE_PARALLEL_ANALYZE
#define _USE_PARALLEL_IDENT

#ifndef _ANALYZE_PARALLEL_SCALE
#define _ANALYZE_PARALLEL_SCALE 1
#endif

#ifndef _IDENT_PARALLEL_SCALE
#define _IDENT_PARALLEL_SCALE 1
#endif

#define _USE_MATH_DEFINES
#include <cmath>
#include <thread>

#include "fft.h"
#include "onset.h"

namespace DMA::Onset {
	void __analyze(std::span<float> in, std::span<float> out, float* max) {
		for (int i = 0; i < out.size(); i++) {
			for (int j = 0; j < FFT::WINDOW_SIZE / 2; j++) {
				float sample = in[i * (FFT::WINDOW_SIZE / 2) + j];
				out[i] += pow(sample * j, 2);
			}

			if (out[i] > *max) {
				*max = out[i];
			}
		}
	}

	void analyze(std::vector<float>& in, std::vector<float>& out) {
		out.resize(in.size() / (FFT::WINDOW_SIZE / 2), 0);

#ifdef _USE_PARALLEL_ANALYZE
		int num_threads = std::thread::hardware_concurrency() * _ANALYZE_PARALLEL_SCALE;
		int thread_samples = ceil((double)out.size() / num_threads);
		std::vector<float> maxs(num_threads, 0);
		std::vector<std::thread> threads;
		threads.reserve(num_threads);

		for (int i = 0; i < num_threads; i++) {
			threads.emplace_back([&, thread_samples, i]() {
				int start = i * thread_samples;
				int end = (i + 1) * thread_samples;

				if (end > out.size()) {
					end = out.size();
				}

				std::span<float> analyze_in(in.data() + start * (FFT::WINDOW_SIZE / 2), (end - start)* (FFT::WINDOW_SIZE / 2));
				std::span<float> analyze_out(out.data() + start, end - start);

				__analyze(analyze_in, analyze_out, &maxs[i]);
				}
			);
		}

		for (auto& thread : threads) {
			thread.join();
		}

		float max = 0;
		for (int i = 0; i < maxs.size(); i++) {
			if (maxs[i] > max) {
				max = maxs[i];
			}
		}
#else
		float max = 0;
		__analyze(in, out, &max);
#endif

		for (int i = 0; i < out.size(); i++) {
			out[i] = pow(out[i] / max, 2);
		}
	}

	void detect(std::vector<float>& in, std::vector<int>& starts, std::vector<int>& stops) {
		int start_idx = 0;
		int stop_idx = 0;

		for (int i = 0; i < in.size(); i++) {
			if (start_idx > stop_idx) {
				if (in[i] < ONSET_THRESHOLD) {
					stops.push_back(i);
					stop_idx++;
				}
			}
			else if (start_idx == stop_idx) {
				if (in[i] > ONSET_THRESHOLD) {
					starts.push_back(i);
					start_idx++;
				}
			}
		}

		if (start_idx > stop_idx) {
			stops.push_back(in.size() - 1);
		}
	}

	void __identify(std::span<complex> in, int start, int stop, float sampling_rate, float* out) {
		std::span<complex> fft_in(in.data() + start * FFT::WINDOW_SIZE / 2, (stop - start) * FFT::WINDOW_SIZE / 2);
		std::vector<complex> fft_out;
		FFT::fft(fft_in, fft_out);

		float max_value = 0;
		int max_index = -1;
		for (int j = 1; j < fft_out.size() / 2; j++) {
			float magnitude = std::abs(fft_out[j]);
			if (magnitude > max_value) {
				max_value = magnitude;
				max_index = j;
			}
		}

		for (int div = 6; div > 1; div--) {
			float harmonic = std::abs(fft_out[max_index / div]);
			if (harmonic / max_value > HARMONIC_RATIO_THRESHOLD) {
				max_index /= div;
				max_value = harmonic;
			}
		}

		*out = max_index * sampling_rate / fft_out.size();
	}

	void identify(std::span<complex> in, std::span<int> starts, std::span<int> stops, float sampling_rate, std::vector<float>& out) {
		out.resize(starts.size());

#ifdef _USE_PARALLEL_IDENT
		int num_threads = std::thread::hardware_concurrency() / _IDENT_PARALLEL_SCALE;
		int thread_notes = ceil((double)starts.size() / num_threads);
		std::vector<std::thread> threads;
		threads.reserve(num_threads);

		for (int i = 0; i < num_threads; i++) {
			threads.emplace_back([&, thread_notes, i]() {
				for (int j = i * thread_notes; j < (i + 1) * thread_notes; j++) {
					if (j > starts.size() - 1) {
						break;
					}
					__identify(in, starts[j], stops[j], sampling_rate, &out[j]);
				}}
			);
		}

		for (auto& thread : threads) {
			thread.join();
		}
#else
		for (int i = 0; i < starts.size(); i++) {
			__identify(in, starts[i], stops[i], sampling_rate, &out[i]);
		}
#endif
	}
}