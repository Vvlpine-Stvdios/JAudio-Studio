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
#include <map>

namespace JAudio::BMS {

	typedef struct {
		int start;
		int duration;
		u8  track;
		u8  note;
		u8  voice;
		u8  velocity;
	} NoteEvent;

	class Parser {

		public:
			bool  loadFromFile(const std::string &filepath);
			const std::vector<NoteEvent> &getNotes    () const { return m_notes;    }
			const int                    &getPPQN     () const { return m_ppqn;     }
			const std::map<int, int>     &getTempoMap () const { return m_tempoMap; }
		
		private:
			std::vector<NoteEvent> m_notes;
			int                    m_ppqn;
			std::map   <int, int>  m_tempoMap;

	};

};