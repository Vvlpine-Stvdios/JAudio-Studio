/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : JAudio/Core/bases.hpp
 * 	Project : libJAudio
 * 
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#pragma once

#include <JAudio/Core/Types>

#include <string>
#include <vector>
#include <map>
#include <variant>

namespace MIDI {

	struct TrackInfo {

		std::string name;
		bool        isPerc;

	};

};

namespace JAudio::BMS {

	struct NoteEvent {

		int start;
		int duration;
		u8  track;
		u8  note;
		u8  voice;
		u8  velocity;

	};

	struct AutomationEvent {

		int             start;
		std::vector<u8> args;

	};

	struct Sequence {

		std::vector<NoteEvent> notes;
		std::vector<int>       tracks;
		std::map   <u8, std::map<u8, std::vector<JAudio::BMS::AutomationEvent>>> automation;
		int PPQN = 120;

		void clear() {
			notes      .clear();
			tracks     .clear();
			automation .clear();

			PPQN = 120;
		}

	};
	
};

namespace JAudio::AFC {

	struct Stream {

		u32 size;
		u32 numberOfSamples;
		u16 sampleRate;
		u16 bitsPerSample;
		u16 samplesPerFrame;
		u16 unknown0x0E;
		u32 loopFlag;
		u32 loopStart;
		u64 padding;

		std::vector<s16> samples;

		void clear() {
			samples.clear();
		}

	};

};

namespace JAudio::Core {

	using ParsedData = std::variant<
		std        ::monostate,
		JAudio::BMS::Sequence,
		JAudio::AFC::Stream
	>;

	using ExportData = std::variant<
		std::monostate,
		std::vector<MIDI::TrackInfo>
	>;

	class IParser {
		public:
			virtual ~IParser() = default;

			virtual bool loadFromFile(const std::string &filePath, ParsedData &outData) = 0;
	};


	class IExporter {
		public:
			virtual ~IExporter() = default;

			virtual bool exportToFile(const std::string &filePath, const ParsedData &parsedData, const ExportData &exportData = std::monostate()) = 0;
	};

};