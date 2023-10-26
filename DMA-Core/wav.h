#pragma once

namespace DMA::Audio {
	struct WAVHeader {
		char chunk_id[4];
		uint32_t chunk_size;
		char format[4];
		char fmt_chunk_id[4];
		uint32_t fmt_chunk_size;
		uint16_t audio_format;
		uint16_t num_channels;
		uint32_t sample_rate;
		uint32_t byte_rate;
		uint16_t block_align;
		uint16_t sample_size;
		char data_chunk_id[4];
		uint32_t data_chunk_size;
	};

	class WAV {
	public:
		/// <summary>
		/// Constructs an empty WAV object
		/// </summary>
		WAV() = default;

		/// <summary>
		/// Constructs a WAV object from the given file
		/// </summary>
		/// <param name="file">The file to read from</param>
		WAV(const wchar_t* file);

		/// <summary>
		/// Assigns the WAV object from another WAV object
		/// </summary>
		WAV& operator=(WAV&& other) noexcept;

		/// <summary>
		/// Destroys the WAV object
		/// </summary>
		~WAV();

		/// <summary>
		/// Returns a pointer to the audio data
		/// </summary>
		/// <returns>A pointer to the audio data</returns>
		const char* data() const { return _data; }

		/// <summary>
		/// Returns the number of samples in the audio data
		/// </summary>
		/// <returns></returns>
		size_t num_samples() const { return 8 * _header.data_chunk_size / _header.sample_size; }

		/// <summary>
		/// Returns the size of the audio data in bytes
		/// </summary>
		/// <returns>The size of the audio data</returns>
		size_t size() const { return _header.data_chunk_size; }

		/// <summary>
		/// Returns the size of a single sample in bytes
		/// </summary>
		/// <returns>The size of a single sample</returns>
		size_t sample_size() const { return _header.sample_size / 8; }

		/// <summary>
		/// Returns the sampling rate of the audio data
		/// </summary>
		/// <returns>The sampling rate of the audio data</returns>
		uint32_t sample_rate() const { return _header.sample_rate; }

		/// <summary>
		/// Returns a sample from the audio data at the given index
		/// </summary>
		float operator [](const size_t& index) const;
	private:
		WAVHeader _header = { 0 };
		char* _data = nullptr;
	};
}