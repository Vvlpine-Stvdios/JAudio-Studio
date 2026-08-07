/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : JAudio/BMS/parser.hpp
 * 	Project : libJAudio
 * 
 ********************************************************************************************************************/
#pragma once

#include <JAudio/BMS/Commands>

#include <cstdint>
#include <string>
#include <vector>

namespace JAudio::BMS {

	typedef struct {
		int start;
		int duration;
		u8  track;
		u8  note;
		u8  voice;
		u8  velocity;
	} BMSNoteEvent;

	class BMSParser {

		public:
			bool                           loadFromFile (const std::string &filepath);
			const std::vector<BMSNoteEvent> &getCommands  () const { return m_notes; }
		
		public:
			std::vector<BMSNoteEvent> m_notes;

	};

};