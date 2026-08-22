/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/20
 * 	File    : JAudio/AFC/decoder.hpp
 * 	Project : libJAudio
 * 
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#pragma once

#include <vector>
#include <JAudio/Core/Types>
#include <JAudio/Core/Bases>

namespace JAudio::AFC {

	const s16 AFC_COEFFICIENTS[16][2] = {
		{     0,     0 },
		{  2048,     0 },
		{     0,  2048 },
		{  1024,  1024 },
		{  4096, -2048 },
		{  3584, -1536 },
		{  3072, -1024 },
		{  4608, -2560 },
		{  4200, -2248 },
		{  4800, -2300 },
		{  5120, -3072 },
		{  2048, -2048 },
		{  1024, -1024 },
		{ -1024,  1024 },
		{ -1024,     0 },
		{ -2048,     0 },
	};

	class Decoder : public JAudio::Core::IParser {
		public:
			bool loadFromFile(const std::string &filePath, JAudio::Core::ParsedData &outData) override;

	};

};