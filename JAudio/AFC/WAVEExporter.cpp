/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/20
 * 	File    : JAudio/AFC/WAVEExporter.cpp
 * 	Project : libJAudio
 * 
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#include <JAudio/AFC/WAVEExporter>
#include <JAudio/Core/IO>
#include <JAudio/AFC/Decoder>

#include <fstream>
#include <iostream>
#include <vector>
#include <variant>

bool PCM::WAVEExporter::exportToFile(const std::string &filePath, const JAudio::Core::ParsedData &parsedData, const JAudio::Core::ExportData &exportData) {
	JAudio::AFC::Stream stream;

	if (std::holds_alternative<JAudio::AFC::Stream>(parsedData)) {
		stream = std::get<JAudio::AFC::Stream>(parsedData);
	} else {
		std::cerr << "Parsed data is not of type JAudio::AFC::Stream!" << std::endl;
		return false;
	}

	if (!std::holds_alternative<std::monostate>(exportData)) {
		std::cout << "Export data is not used for PCM::WAVEExporter. Pass in std::monostate." << std::endl;
		return false;
	}

	std::ofstream   file   = std::ofstream(filePath, std::ios::out | std::ios::binary);
	std::vector<u8> buffer = { };

	if (!file) {
		std::cerr << "Failed to open source file '" << filePath << "'" << std::endl;
		return false;
	}

	// 
	// HEADER
	// 

	uint32_t dataChunkSize = stream.samples.size() * sizeof(s16);
	uint32_t smplChunkSize = stream.loopFlag ? 60 : 0;
	uint32_t fileSize      = 36 + dataChunkSize + (stream.loopFlag ? 8 + smplChunkSize : 0);

	JAudio::Core::IO::write     (buffer, std::vector<u8>({ 'R', 'I', 'F', 'F' }));
	JAudio::Core::IO::write<u32>(buffer, fileSize);
	JAudio::Core::IO::write     (buffer, std::vector<u8>({ 'W', 'A', 'V', 'E' }));

	// fmt Chunk
	JAudio::Core::IO::write     (buffer, std::vector<u8>({ 'f', 'm', 't', ' ' }));
	JAudio::Core::IO::write<u32>(buffer, 16                                     ); // Chunk size
	JAudio::Core::IO::write<u16>(buffer,  1                                     ); // PCM
	JAudio::Core::IO::write<u16>(buffer,  2                                     ); // Channels
	JAudio::Core::IO::write<u32>(buffer, stream.sampleRate                      ); 
	JAudio::Core::IO::write<u32>(buffer, stream.sampleRate * 2 * sizeof(s16)    ); // Byte rate
	JAudio::Core::IO::write<u16>(buffer,                     2 * sizeof(s16)    ); // Block align
	JAudio::Core::IO::write<u16>(buffer, 16                                     ); // Bits per sample

	// 
	// DATA
	// 

	JAudio::Core::IO::write     (buffer, std::vector<u8>({ 'd', 'a', 't', 'a' }));
	JAudio::Core::IO::write<u32>(buffer, dataChunkSize                          );

	for (int16_t sample : stream.samples) {
		JAudio::Core::IO::write<u16>(buffer, static_cast<u16>(sample));
	}

	// 
	// LOOPING
	// 

	if (stream.loopFlag) {
		JAudio::Core::IO::write     (buffer, std::vector<u8>({ 's', 'm', 'p', 'l' }));
		JAudio::Core::IO::write<u32>(buffer, smplChunkSize                          );
		JAudio::Core::IO::write<u32>(buffer,          0                             ); // Manufacturer
		JAudio::Core::IO::write<u32>(buffer,          0                             ); // Product
		JAudio::Core::IO::write<u32>(buffer, 1000000000 / stream.sampleRate         ); // Sample Period
		JAudio::Core::IO::write<u32>(buffer,         60                             ); // MIDI Unity Note
		JAudio::Core::IO::write<u32>(buffer,          0                             ); // MIDI Pitch Fraction
		JAudio::Core::IO::write<u32>(buffer,          0                             ); // SMPTE Format
		JAudio::Core::IO::write<u32>(buffer,          0                             ); // SMPTE Offset
		JAudio::Core::IO::write<u32>(buffer,          1                             ); // Num Sample Loops
		JAudio::Core::IO::write<u32>(buffer,          0                             ); // Sampler Data

		// Loop Data Structure
		JAudio::Core::IO::write<u32>(buffer, 0                     ); // Identifier
		JAudio::Core::IO::write<u32>(buffer, 0                     ); // Type (0 = Forward)
		JAudio::Core::IO::write<u32>(buffer, stream.loopStart      ); // Start
		JAudio::Core::IO::write<u32>(buffer, stream.numberOfSamples); // End
		JAudio::Core::IO::write<u32>(buffer, 0                     ); // Fraction
		JAudio::Core::IO::write<u32>(buffer, 0                     ); // Play Count (0 = Infinite)
	}

	// 
	// CLEANUP
	// 

	file.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
	bool success = file.good();

	file.close();
	return success;
}