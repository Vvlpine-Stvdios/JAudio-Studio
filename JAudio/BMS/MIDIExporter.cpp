/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/10
 * 	File    : JAudio/BMS/MIDIExporter.cpp
 * 	Project : libJAudio
 *  
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#include <JAudio/BMS/MIDIExporter>
#include <JAudio/Core/IO>
#include <JAudio/Core/Bases>
#include <JAudio/Core/Endian>

#include <iostream>
#include <algorithm>
#include <variant>

bool MIDI::Exporter::exportToFile(const std::string &filePath, const JAudio::Core::ParsedData &parsedData, const JAudio::Core::ExportData &exportData) {
	JAudio::BMS::Sequence        sequence;
	std::vector<MIDI::TrackInfo> trackInfos;

	if (std::holds_alternative<JAudio::BMS::Sequence>(parsedData)) {
		sequence = std::get<JAudio::BMS::Sequence>(parsedData);
	} else {
		std::cerr << "Parsed data is not of type JAudio::BMS::Sequence!" << std::endl;
		return false;
	}

	if (std::holds_alternative<std::vector<MIDI::TrackInfo>>(exportData)) {
		trackInfos = std::get<std::vector<MIDI::TrackInfo>>(exportData);
	} else {
		std::cerr << "Export data is not of type MIDI::TrackInfo!" << std::endl;
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

	// 0x4D746864 = MThd in ASCII
	// 0x00000006 = length of header
	//⎡0x0001---- = format
	//⎣0x----XXXX = PPQN
	// 0xXXXX---- = # Tracks (add one for the tempo track)

	JAudio::Core::IO::write     (buffer, std::vector<u8>({ 'M', 'T', 'h', 'd' }));
	JAudio::Core::IO::write<u32>(buffer, (u32)0x00000006,                 JAudio::Core::ENDIAN::BIG);
	JAudio::Core::IO::write<u16>(buffer, (u16)0x0001,                     JAudio::Core::ENDIAN::BIG);
	JAudio::Core::IO::write<u16>(buffer, (u16)sequence.tracks.size() + 1, JAudio::Core::ENDIAN::BIG);
	JAudio::Core::IO::write<u16>(buffer, (u16)sequence.PPQN,              JAudio::Core::ENDIAN::BIG);
	
	std::cout << "Wrote header" << std::endl;

	// 
	// TEMPO TRACK
	// 

	std::vector<u8> tempoTrackBuffer = { };

	JAudio::Core::IO::write(tempoTrackBuffer, std::vector<u8>({ 'M', 'T', 'r', 'k' }));
	int lastTick = 0;

	for (const JAudio::BMS::AutomationEvent &event : sequence.automation.at(0xFF).at(JAudio::BMS::CMD_TEMPO_SET)) {
		int deltaTick = event.start - lastTick;
		lastTick      = event.start;

		writeVariableLength(tempoTrackBuffer, deltaTick);

		int microsecondsPerBeat = 60000000 / (float)event.args[1];

		JAudio::Core::IO::write<u24>(tempoTrackBuffer, (u24)0xFF5103,            JAudio::Core::ENDIAN::BIG);
		JAudio::Core::IO::write<u24>(tempoTrackBuffer, (u24)microsecondsPerBeat, JAudio::Core::ENDIAN::BIG);
	}

	std::cout << "Wrote global track" << std::endl;

	// 0x00 - delta time
	// 0xFF 2F 00 - end track
	JAudio::Core::IO::write<u32>(tempoTrackBuffer, (u32)0x00FF2F00, JAudio::Core::ENDIAN::BIG);

	std::vector<u8> tempoTrackSize = { };
	JAudio::Core::IO::write(tempoTrackSize, (u32)tempoTrackBuffer.size() - 4, JAudio::Core::ENDIAN::BIG);

	tempoTrackBuffer.insert(tempoTrackBuffer.begin() + 4, tempoTrackSize.begin(), tempoTrackSize.end());

	buffer.insert(buffer.end(), tempoTrackBuffer.begin(), tempoTrackBuffer.end());

	// 
	// TRACKS
	// 

	std::vector<JAudio::BMS::NoteEvent> events = { };

	int i = 0;
	for (const int &track : sequence.tracks) {
		events.clear();

		for (const JAudio::BMS::NoteEvent &note : sequence.notes) {
			if (note.track == track) {
				events.push_back(note);
			}
		}

		writeMIDITrack(buffer, events, trackInfos[i++]);
		std::cout << "Wrote track " << (int)track << std::endl;
	}

	// 
	// CLEANUP
	// 

	file.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
	bool success = file.good();

	file.close();
	return success;
}

void MIDI::Exporter::writeMIDITrack(std::vector<u8> &buffer, const std::vector<JAudio::BMS::NoteEvent> &events, const TrackInfo &trackInfo) {
	std::vector<u8> trackBuffer = { };
	
	JAudio::Core::IO::write(trackBuffer, std::vector<u8>({ 'M', 'T', 'r', 'k' }));

	enum TYPE { START = 0x90, END = 0x80, };

	struct Info {

		u32  start;
		u8   note;
		u8   velocity;
		u8   channel;
		TYPE type;

	};

	// Get the name as an array of bytes
	std::vector<u8> name(
        reinterpret_cast<const u8*>(trackInfo.name.c_str()),
        reinterpret_cast<const u8*>(trackInfo.name.c_str() + std::strlen(trackInfo.name.c_str()))
	);

	size_t size = std::strlen(trackInfo.name.c_str());
	
	// Wait 0
	// Instrument Name
	JAudio::Core::IO::write<u24> (trackBuffer, (u24)0x00FF04, JAudio::Core::ENDIAN::BIG);
	writeVariableLength          (trackBuffer, size);
	
	trackBuffer.insert(trackBuffer.end(), name.begin(), name.end());

	// Wait 0
	// Track Name
	JAudio::Core::IO::write<u24> (trackBuffer, (u24)0x00FF03, JAudio::Core::ENDIAN::BIG);
	writeVariableLength          (trackBuffer, size);

	trackBuffer.insert(trackBuffer.end(), name.begin(), name.end());

	std::vector<Info> MIDIEvents = { };

	for (const JAudio::BMS::NoteEvent &event : events) {
		u8 track = (trackInfo.isPerc) ? 9 : (event.track > 8) ? event.track + 1 : event.track;
		MIDIEvents.push_back((Info) {
			static_cast<u32>(event.start),
			event.note,
			event.velocity,
			track,
			START
		});

		MIDIEvents.push_back((Info) {
			static_cast<u32>(event.start + event.duration),
			event.note,
			0x40,
			track,
			END
		});
	}

	std::sort(MIDIEvents.begin(), MIDIEvents.end(), [](const Info &a, const Info &b) {
		return a.start < b.start;
	});

	int lastTick = 0;

	for (const Info &event : MIDIEvents) {
		int deltaTick = event.start - lastTick;
		lastTick     += deltaTick;

		writeVariableLength(trackBuffer, deltaTick);

		trackBuffer.push_back((u8)event.type | event.channel);
		trackBuffer.push_back((u8)event.note);
		trackBuffer.push_back((u8)event.velocity);
	}

	// 00 - delta time
	// FF - Meta event
	// 2F - Track end
	// 00 - 0bytes data
	JAudio::Core::IO::write<u32>(trackBuffer, (u32)0x00FF2F00, JAudio::Core::ENDIAN::BIG);

	std::vector<u8> trackSize = { };
	JAudio::Core::IO::write(trackSize, (u32)trackBuffer.size() - 4, JAudio::Core::ENDIAN::BIG);

	trackBuffer.insert(trackBuffer.begin() + 4, trackSize.begin(), trackSize.end());

	buffer.insert(buffer.end(), trackBuffer.begin(), trackBuffer.end());
}

void MIDI::Exporter::writeVariableLength(std::vector<u8> &trackBuffer, u32 value) {
    u32 buffer = value & 0x7F;
    
    while ((value >>= 7)) {
        buffer <<= 8;
        buffer |= ((value & 0x7F) | 0x80);
    }
    
    while (true) {
        trackBuffer.push_back(buffer & 0xFF);

        if (buffer & 0x80) { buffer >>= 8; }
		else               { break; }
    }
}