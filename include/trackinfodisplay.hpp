/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/15
 * 	File    : trackinfodisplay.hpp
 * 	Project : JAudio Studio
 * 
 *	JAudio Studio - Uses libJAudio to parse and convert JAudio files into standard formats, and displays that data.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#pragma once

#include <JAudio/Core/Types>
#include <JAudio/BMS/MIDIExporter>

#include <QListWidgetItem>

#include <string>

class TrackInfoDisplay : public QWidget {

	Q_OBJECT

	public:
		explicit TrackInfoDisplay(QWidget *parent = nullptr, int index = -1, const std::string &name = "");

		constexpr void setName   (const std::string &name  ) { m_info.name     = name;   }
		constexpr void setIsPerc (const bool        &isPerc) { m_info.isPerc   = isPerc; }

		const std::string     &getName   () const { return m_info.name;     }
		const bool            &getIsPerc () const { return m_info.isPerc;   }
		const MIDI::TrackInfo &getInfo   () const { return m_info;          }

	private:
		MIDI::TrackInfo m_info { };

};