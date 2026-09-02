/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/20
 * 	File    : JAudio/ADPCM/ADPCM.cpp
 * 	Project : libJAudio
 * 
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#include <JAudio/ADPCM/ADPCM>
#include <JAudio/Core/IO>

#include <algorithm>
#include <iostream>

void ADPCM::Decoder::decodeFrame(const ADPCM::Frame &frame, ADPCM::History &history, const s16 coefficients[16][2], std::vector<s16> &out) {
	int scale = 1 << frame.scaleFactor();

	for (int i = 0; i < 16; i++) {
		int32_t sample = frame[i];

		sample = (sample * scale) << 11;
		sample = (sample + history.predict(coefficients[frame.predicatorIndex()])) >> 11;
		sample = std::clamp(sample, INT16_MIN, INT16_MAX);

		history.update(sample);

		out.push_back(static_cast<int16_t>(sample));
	}
}

void ADPCM::Decoder::decode(const std::vector<u8> &data, const u32 &numberOfSamples, const s16 coefficients[16][2], bool isStereo, std::vector<s16> &out, size_t offset) {
	if (isStereo) {
		out.reserve(numberOfSamples * 2);

		ADPCM::History LHistory;
		ADPCM::History RHistory;

		ADPCM::Frame LFrame;
		ADPCM::Frame RFrame;

		std::vector<s16> LSamples; LSamples.reserve(16);
		std::vector<s16> RSamples; RSamples.reserve(16);

		for (int samplesDecoded = 0; samplesDecoded < numberOfSamples; offset += 18) {
			LFrame.header = JAudio::Core::IO::read<u8>(data, offset + 0);
			RFrame.header = JAudio::Core::IO::read<u8>(data, offset + 9);

			std::copy_n(data.begin() + offset +  1, 8, reinterpret_cast<char *>(LFrame.samples));
			std::copy_n(data.begin() + offset + 10, 8, reinterpret_cast<char *>(RFrame.samples));

			LSamples.clear();
			RSamples.clear();

			decodeFrame(LFrame, LHistory, coefficients, LSamples);
			decodeFrame(RFrame, RHistory, coefficients, RSamples);

			int samplesToCopy = std::min<u32>(16, numberOfSamples - samplesDecoded);
			
			for (int s = 0; s < samplesToCopy; s++) {
				out.push_back(LSamples[s]);
				out.push_back(RSamples[s]);
			}

			samplesDecoded += samplesToCopy;
		}
	} else {
		out.reserve(numberOfSamples);

		ADPCM::History   history = { .last = 0, .penult = 0 };
		ADPCM::Frame     frame;
		std::vector<s16> samples; samples.reserve(16);

		for (int samplesDecoded = 0; samplesDecoded < numberOfSamples; samplesDecoded += 16, offset += 18) {
			frame = { .header = JAudio::Core::IO::read<u8>(data, offset), .samples = { } };

			JAudio::Core::IO::read(data, offset + 1, 8, frame.samples);

			decodeFrame(frame, history, coefficients, samples);

			out.insert(out.end(), samples.begin(), samples.end());
		}
	}
}