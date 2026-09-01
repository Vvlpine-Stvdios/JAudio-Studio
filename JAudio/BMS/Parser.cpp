/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : JAudio/BMS/parser.cpp
 * 	Project : libJAudio
 *  
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#include <JAudio/Core/Types>
#include <JAudio/Core/Endian>
#include <JAudio/BMS/Parser>

#include <fstream>
#include <iostream>
#include <variant>
#include <tuple>

#ifndef DDEBUG
	#define OPC "0x" << std::uppercase << std::setw(2) << (int)
	#define HEX(n)      std::uppercase << std::setw(n) << (int)
	#define PTR "0x" << std::uppercase << std::setw(6) << (int)
#endif

bool JAudio::BMS::Parser::loadFromFile(const std::string &filePath, JAudio::Core::ParsedData &outData) {
	std::ifstream file(filePath, std::ios::binary);

	JAudio::BMS::Sequence sequence;

	if (!file) { std::cout << "Failed to parse file! File does not exist." << std::endl; return false; }

	std::vector<u8> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	std::vector<size_t> stack       = { };
	int                 globalClock = 0;
	size_t              i           = 0;

	struct ActiveNode {
		NoteEvent note;
		bool      active = false;
	};

	std::array<std::array<ActiveNode, (size_t)7>, 256> notes;
	u8 currentTrack = 0xFF;

	#ifndef DDEBUG
		// Set up debug prints
		std::cout << std::hex << std::setfill('0');
	#endif

	bool error = false;

	std::cout << "Reading..." << std::endl;

	while (i < buffer.size() && !error) {
		u8 opcode = buffer[i];

		if (CMD_NOTE_ON_BEGIN <= opcode && opcode <= CMD_NOTE_ON_END) {
			// Sanity check
			if (i + 2 < buffer.size()) {
				u8 note     = opcode;
				u8 voice    = buffer[i + 1];
				u8 velocity = buffer[i + 2];

				if (voice < 8) {
					notes[currentTrack][voice - 1] = {
						{ globalClock, -1, currentTrack, note, voice, velocity }, true
					};
				}
			} else { break; }

			i += 3;
			continue;
		}

		if (CMD_NOTE_OFF_BEGIN <= opcode && opcode <= CMD_NOTE_OFF_END) {
			u8 voice = opcode & 0x0F;

			if (voice < 8 && notes[currentTrack][voice - 1].active) {
				notes[currentTrack][voice - 1].note.duration = globalClock - notes[currentTrack][voice - 1].note.start;
				notes[currentTrack][voice - 1].active        = false;

				sequence.notes.push_back(notes[currentTrack][voice - 1].note);
			}

			i++;
			continue;
		}

		switch (opcode) {
			case CMD_WAIT_BYTE:
				if (i + 1 < buffer.size()) {
					globalClock += buffer[i + 1];

					i += 2;
				} else { error = true; }
				break;
			
			case CMD_WAIT_SHORT:
				if (i + 2 < buffer.size()) {
					const u8 duration[2] = { buffer[i + 1], buffer[i + 2] };
					globalClock         += JAudio::Core::swapEndian<u16>(duration);

					i += 3;
				} else { error = true; }
				break;
			
			case CMD_TRACK_POINTER:
				if (i + 5 < buffer.size()) {
					currentTrack  = (u8)buffer[i + 1];
					const u8 p[3] = { buffer[i + 2], buffer[i + 3], buffer[i + 4] };

					u24 pointer = JAudio::Core::swapEndian<u24>(p);

					i += 5;

					stack.push_back(i);
					i = (size_t)pointer;

					sequence.tracks.push_back(currentTrack);
				} else { error = true; }
				break;
			
			case CMD_CALL:
				if (i + 5 < buffer.size()) {
					u8 offset = (buffer[i + 1] == 0xC0) ? 1 : 0;

					const u8 p[4] = {
						buffer[i + offset + 1],
						buffer[i + offset + 2],
						buffer[i + offset + 3],
						buffer[i + offset + 4],
					};

					u32 pointer = JAudio::Core::swapEndian<u32>(p);

					i += offset + 5;

					stack.push_back(i);
					i = (size_t)pointer;
				} else { error = true; }
				break;
			
			case CMD_END_TRACK:
				// Not really an error, but marks the end of our read
				if (currentTrack == 0xFF || stack.empty()) { error = true; break; }
				currentTrack = 0xFF;
				globalClock  = 0;

			case CMD_RETURN:
				if (stack.size() != 0) {
					i = stack.back();
					stack.pop_back();
				} else { error = true; }
				break;
			
			case CMD_JUMP:
				if (i + 5 < buffer.size()) {
					const u8 p[3]    = { buffer[i + 2], buffer[i + 3], buffer[i + 4] };
					u24      pointer = JAudio::Core::swapEndian<u24>(p);

					i += 5;
				} else { error = true; }
				break;
			
			case CMD_PPQN_SET:
				if (i + 3 < buffer.size()) {
					sequence.PPQN = (u8)buffer[i + 2];

					i += 3;
				} else { error = true; }
				break;
			
			case CMD_NOOP:
				i++;
				break;
			
			case 0xF1:
			case 0xF4:
				if (i + 2 < buffer.size()) {
					sequence.automation[currentTrack][opcode].push_back(
						(AutomationEvent) {
							globalClock, {
								(u8)buffer[i + 1]
							}
						}
					);

					i += 2;
				} else { error = true; }
				break;

			case CMD_DYNAMICS_SET        :
			case 0xA0                    :
			case 0xA1                    :
			case CMD_PROGRAM_CHANGE      :
			case 0xA6                    :
			case 0xA7                    :
			case 0xCB                    :
			case 0xCC                    :
			case 0xD2                    :
			case CMD_DYNAMIC_TRACK_LABEL :
			case CMD_MODULATION_SET      :
			case CMD_TRACK_HEADER        :
			case CMD_TEMPO_SET           :
				if (i + 3 < buffer.size()) {
					sequence.automation[currentTrack][opcode].push_back(
						(AutomationEvent) {
							globalClock, {
								(u8)buffer[i + 1],
								(u8)buffer[i + 2],
							}
						}
					);

					i += 3;
				} else { error = true; }
				break;
			
			case CMD_PAN_SET   :
			case CMD_PARAM_SET :
			case 0xAC          :
			case 0xAD          :
			case 0xDD          :
			case 0xEF          :
				if (i + 4 < buffer.size()) {
					sequence.automation[currentTrack][opcode].push_back(
						(AutomationEvent) {
							globalClock, {
								(u8)buffer[i + 1],
								(u8)buffer[i + 2],
								(u8)buffer[i + 3],
							}
						}
					);

					i += 4;
				} else { error = true; }
				break;
			
			case CMD_PARAM_SET_OVER_TIME:
				if (i + 5 < buffer.size()) {
					sequence.automation[currentTrack][opcode].push_back(
						(AutomationEvent) {
							globalClock, {
								(u8)buffer[i + 1],
								(u8)buffer[i + 2],
								(u8)buffer[i + 3],
								(u8)buffer[i + 4],
							}
						}
					);
					
					i += 5;
				} else { error = true; }
				break;
			
			default:
				std::cout << "[WARNING]: Came across unhandled opcode `" << OPC opcode << " at " << PTR i << std::endl;
				error = true;
				break;
		}
	}

	file.close();

	std::cout << std::dec << std::setfill(' ');
	std::cout << "Finished parsing \"" << filePath << "\" successfully." << std::endl;

	outData = sequence;
	
	return true;
}