/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/20
 * 	File    : JAudio/AFC/Decoder.cpp
 * 	Project : libJAudio
 * 
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#include <JAudio/AFC/Decoder>
#include <JAudio/ADPCM/ADPCM>
#include <JAudio/Core/Endian>
#include <JAudio/Core/IO>

#include <iostream>
#include <fstream>

bool JAudio::AFC::Decoder::loadFromFile(const std::string &filePath, JAudio::Core::ParsedData &outData) {
	std::ifstream file(filePath, std::ios::binary);

	JAudio::AFC::Stream stream;

	if (!file) { std::cout << "Failed to parse file! File does not exist." << std::endl; return false; }

	std::vector<u8> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	stream = {
		.size            = JAudio::Core::IO::read<u32>(buffer, 0x00),
		.numberOfSamples = JAudio::Core::IO::read<u32>(buffer, 0x04),
		.sampleRate      = JAudio::Core::IO::read<u16>(buffer, 0x08),
		.bitsPerSample   = JAudio::Core::IO::read<u16>(buffer, 0x0A),
		.samplesPerFrame = JAudio::Core::IO::read<u16>(buffer, 0x0C),
		.unknown0x0E     = JAudio::Core::IO::read<u16>(buffer, 0x0E),
		.loopFlag        = JAudio::Core::IO::read<u32>(buffer, 0x10),
		.loopStart       = JAudio::Core::IO::read<u32>(buffer, 0x14),
		.padding         = JAudio::Core::IO::read<u64>(buffer, 0x18),
		.samples         = {  }
	};

	ADPCM::Decoder decoder;

	decoder.decode(buffer, stream.numberOfSamples, AFC_COEFFICIENTS, true, stream.samples, 0x20);

	outData = stream;

	return true;
}