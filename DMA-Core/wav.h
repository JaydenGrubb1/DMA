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
		/// Constructs a WAV object from the given file
		/// </summary>
		/// <param name="file">The file to read from</param>
		WAV(const char* file);

		/// <summary>
		/// Destroys the WAV object
		/// </summary>
		~WAV();

		/// <summary>
		/// Returns a pointer to the audio data
		/// </summary>
		/// <returns>A pointer to the audio data</returns>
		const char *data() const { return _data; }

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
	private:
		WAVHeader _header;
		char* _data;
	};
}