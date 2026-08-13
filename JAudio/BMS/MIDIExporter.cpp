/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/10
 * 	File    : JAudio/BMS/MIDIExporter.cpp
 * 	Project : libJAudio
 *  
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vulpine Studios
 * 
 ********************************************************************************************************************/

#include <JAudio/BMS/MIDIExporter>
#include <JAudio/Core/Endian>

#include <iostream>
#include <algorithm>

bool MIDI::Exporter::exportToFile(const std::string &filePath, const JAudio::BMS::Parser &parser) {
	std::ofstream file = std::ofstream(filePath, std::ios::out | std::ios::binary);

	std::vector<u8> buffer = { };

	if (!file) {
		std::cerr << "Failed to open source file '" << filePath << "'" << std::endl;
		return false;
	}

	// 
	// HEADER
	// 

	// 0x4D746864 = Mthd in ASCII
	// 0x00000006 = length of header
	//⎡0x0001---- = format
	//⎣0x----XXXX = PPQN
	// 0xXXXX---- = # Tracks (add one for the tempo track)

	writeBytes<u32>(buffer, (u32)0x4D546864                   );
	writeBytes<u32>(buffer, (u32)0x00000006                   );
	writeBytes<u16>(buffer, (u16)0x0001                       );
	// writeBytes<u16>(buffer, (u16)0x0001                       );
	writeBytes<u16>(buffer, (u16)parser.getTracks().size() + 1);
	writeBytes<u16>(buffer, (u16)parser.getPPQN()             );
	
	// 
	// TEMPO TRACK
	// 

	std::vector<u8> tempoTrackBuffer = { };

	writeBytes<u32>(tempoTrackBuffer, 0x4D54726B);
	int lastTick = 0;

	for (const JAudio::BMS::AutomationEvent &event : parser.getAutomation().at(0xFF).at(JAudio::BMS::CMD_TEMPO_SET)) {
		int deltaTick = event.start - lastTick;
		lastTick      = event.start;

		writeVariableLength(tempoTrackBuffer, deltaTick);

		int microsecondsPerBeat = 60000000 / (float)event.args[1];

		writeBytes<u24>(tempoTrackBuffer, (u24)0xFF5103);
		writeBytes<u24>(tempoTrackBuffer, (u24)microsecondsPerBeat);
	}

	// 0x00 - delta time
	// 0xFF 2F 00 - end track
	writeBytes<u32>(tempoTrackBuffer, (u32)0x00FF2F00);

	std::vector<u8> tempoTrackSize = { };
	writeBytes(tempoTrackSize, (u32)tempoTrackBuffer.size() - 4);

	tempoTrackBuffer.insert(tempoTrackBuffer.begin() + 4, tempoTrackSize.begin(), tempoTrackSize.end());

	buffer.insert(buffer.end(), tempoTrackBuffer.begin(), tempoTrackBuffer.end());

	// 
	// TRACKS
	// 

	std::vector<JAudio::BMS::NoteEvent> events = { };

	for (const int &track : parser.getTracks()) {
		events.clear();

		for (const JAudio::BMS::NoteEvent &note : parser.getNotes()) {
			if (note.track == track) {
				events.push_back(note);
			}
		}

		writeMIDITrack(buffer, events);
	}

	// 
	// CLEANUP
	// 

	file.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
	bool success = file.good();

	file.close();
	return success;
}

void MIDI::Exporter::writeMIDITrack(std::vector<u8> &buffer, const std::vector<JAudio::BMS::NoteEvent> &events) {
	std::vector<u8> trackBuffer = { };
	
	// 0x4D54726B = MTrk in ASCII
	writeBytes<u32>(trackBuffer, 0x4D54726B);

	enum TYPE { START = 0x90, END = 0x80, };

	struct Info {

		u32  start;
		u8   note;
		u8   velocity;
		u8   channel;
		TYPE type;

	};

	std::vector<Info> MIDIEvents = { };

	for (const JAudio::BMS::NoteEvent &event : events) {
		MIDIEvents.push_back((Info) {
			static_cast<u32>(event.start),
			event.note,
			event.velocity,
			(u8)((event.track > 8) ? event.track + 1 : event.track),
			START
		});

		MIDIEvents.push_back((Info) {
			static_cast<u32>(event.start + event.duration),
			event.note,
			0x40,
			(u8)((event.track > 8) ? event.track + 1 : event.track),
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
	writeBytes<u32>(trackBuffer, (u32)0x00FF2F00);

	std::vector<u8> trackSize = { };
	writeBytes(trackSize, (u32)trackBuffer.size() - 4);

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

template<JAudio::Core::integral T>
void MIDI::Exporter::writeBytes(std::vector<u8> &trackBuffer, T data) {
	data = JAudio::Core::swapEndian<T>(data);
	for (int i = 0; i < sizeof(T); i++) {
		trackBuffer.push_back((data & 0xFF));
		data >>= 8;
	}
}