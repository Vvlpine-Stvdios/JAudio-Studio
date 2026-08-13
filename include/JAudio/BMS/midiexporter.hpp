/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/10
 * 	File    : JAudio/BMS/midiexporter.hpp
 * 	Project : libJAudio
 * 
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vulpine Studios
 * 
 ********************************************************************************************************************/

#pragma once

#include <JAudio/Core/Types>
#include <JAudio/BMS/Parser>

#include <fstream>
#include <string>
#include <vector>
#include <concepts>

namespace MIDI {

	class Exporter {

		public:
			bool exportToFile(const std::string &filePath, const JAudio::BMS::Parser &parser);
		
		private:
			void writeMIDITrack      (std::vector<u8> &buffer, const std::vector<JAudio::BMS::NoteEvent> &events);
			void writeVariableLength (std::vector<u8> &trackBuffer, u32 value);

			template<JAudio::Core::integral T>
			void writeBytes(std::vector<u8> &trackBuffer, T data);

	};

};