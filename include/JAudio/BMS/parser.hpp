/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : JAudio/BMS/parser.hpp
 * 	Project : libJAudio
 * 
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/
#pragma once

#include <JAudio/Core/Bases>

#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace JAudio::BMS {

	enum OPCODE : u8 {
		CMD_NOOP                = 0x00,
		CMD_NOTE_ON_BEGIN       = 0x01,
		CMD_NOTE_ON_END         = 0x7F,
		CMD_WAIT_BYTE           = 0x80,
		CMD_NOTE_OFF_BEGIN      = 0x81,
		CMD_NOTE_OFF_END        = 0x87,
		CMD_WAIT_SHORT          = 0x88,
		CMD_DYNAMICS_SET        = 0x98,
		CMD_PAN_SET             = 0x9A,
		CMD_PARAM_SET           = 0x9C,
		CMD_PARAM_SET_OVER_TIME = 0x9E,
		CMD_PROGRAM_CHANGE      = 0xA4,
		CMD_TRACK_POINTER       = 0xC1,
		CMD_CALL                = 0xC4,
		CMD_RETURN              = 0xC6,
		CMD_JUMP                = 0xC8,
		CMD_DYNAMIC_TRACK_LABEL = 0xDA,
		CMD_MODULATION_SET      = 0xE6,
		CMD_TRACK_HEADER        = 0xE7,
		CMD_TEMPO_SET           = 0xFD,
		CMD_PPQN_SET            = 0xFE,
		CMD_END_TRACK           = 0xFF,
	};

	class Parser : public JAudio::Core::IParser {
		public:
			bool loadFromFile(const std::string &filePath, JAudio::Core::ParsedData &outData) override;

	};

};