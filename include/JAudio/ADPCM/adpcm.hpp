/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/20
 * 	File    : JAudio/ADPCM/adpcm.hpp
 * 	Project : libJAudio
 * 
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#include <JAudio/Core/Types>
#include <JAudio/Core/Bases>

#include <vector>

namespace ADPCM {

	struct Frame {
	
		u8 header;
		u8 samples[8];

		int scaleFactor     () const { return (header >> 4) & 0x0F; }
		int predicatorIndex () const { return (header >> 0) & 0x0F; }

		int operator [](size_t index) const {
			int sample = (samples[index / 2] >> ((index + 1) % 2) * 4) & 0x0F;
			if (sample >= 8) { sample -= 16; }

			return sample;
		}

	};

	struct History {
		int last   = 0;
		int penult = 0;

		int predict(const s16 coefs[2] ) {
			return coefs[0] * last + coefs[1] * penult;
		}

		void update(int sample) {
			penult = last;
			last   = sample;
		}
	};

	class Decoder {
		public:
		   ~Decoder() = default;
	
			void decode(const std::vector<u8> &data, const u32 &numberOfSamples, const s16 coefficients[16][2], bool isStereo, std::vector<s16> &out, size_t offset = 0);

		private:
			void decodeFrame(const Frame &frame, History &history, const s16 coefficients[16][2], std::vector<s16> &out);
			// void decodeFrame(const u8 *frame, History &ctx, s16 *outBuffer);
	};

};