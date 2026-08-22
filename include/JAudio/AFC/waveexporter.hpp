/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/20
 * 	File    : JAudio/AFC/waveexporter.hpp
 * 	Project : libJAudio
 * 
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#pragma once

#include <JAudio/Core/Types>
#include <JAudio/Core/Bases>

namespace PCM {

	class WAVEExporter : JAudio::Core::IExporter {
		public:
			bool exportToFile(const std::string &filePath, const JAudio::Core::ParsedData &parsedData, const JAudio::Core::ExportData &exportData) override;

	};

};