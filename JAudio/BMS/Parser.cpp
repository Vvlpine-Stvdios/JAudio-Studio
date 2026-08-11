/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : JAudio/BMS/parser.cpp
 * 	Project : libJAudio
 * 
 ********************************************************************************************************************/

#include <JAudio/Core/Types>
#include <JAudio/Core/Endian>
#include <JAudio/BMS/Parser>

#include <fstream>
#include <iostream>

#define OPC "0x" << std::uppercase << std::setw(2)
#define PTR "0x" << std::uppercase << std::setw(6)

bool JAudio::BMS::Parser::loadFromFile(const std::string &filepath) {
	std::ifstream file(filepath, std::ios::binary);

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

	m_notes   .clear();
	m_tracks  .clear();
	m_tempoMap.clear();

	// Set up debug prints
	std::cout << std::hex << std::setfill('0');

	while (i < buffer.size()) {
		u8 opcode = buffer[i];

		// Note On
		if (0x01 <= opcode && opcode <= 0x7F) {
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
		}

		// Wait Byte
		else if (opcode == 0x80) {
			if (i + 1 < buffer.size()) {
				globalClock += buffer[i + 1];

				i += 2;
			} else { break; }
		}

		// Note Off
		else if (0x81 <= opcode && opcode <= 0x87) {
			u8 voice = opcode & 0x0F;

			if (voice < 8 && notes[currentTrack][voice - 1].active) {
				notes[currentTrack][voice - 1].note.duration = globalClock - notes[currentTrack][voice - 1].note.start;
				notes[currentTrack][voice - 1].active        = false;

				m_notes.push_back(notes[currentTrack][voice - 1].note);
			}

			i++;
		}

		// Wait
		else if (opcode == 0x88) {
			if (i + 2 < buffer.size()) {
				const u8 duration[2] = { buffer[i + 1], buffer[i + 2] };
				globalClock         += JAudio::Core::swapEndian<u16>(duration);

				i += 3;

			} else { break; }
		}

		// Track Pointer
		else if (opcode == 0xC1) {
			if (i + 5 < buffer.size()) {
				currentTrack  = (u8)buffer[i + 1];
				const u8 p[3] = { buffer[i + 2], buffer[i + 3], buffer[i + 4] };

				u24 pointer = JAudio::Core::swapEndian<u24>(p);

				i += 5;

				stack.push_back(i);
				i = (size_t)pointer;

				m_tracks.push_back(currentTrack);

			} else { break; }
		}

		// Call
		else if (opcode == 0xC4) {
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

				// std::cout << "Executing Call: " << PTR << (int)pointer << " (Should return to " << PTR << (int)stack.back() << ")" << std::endl;

			} else { break; }
		}

		// Return or Track End
		else if (opcode == 0xC6 || opcode == 0xFF) {
			if (opcode == 0xFF) {
				if (currentTrack == 0xFF || stack.empty()) { break; }
				currentTrack = 0xFF;

				globalClock = 0;
				// std::cout << "Returning: " << PTR << i << std::endl;
			}

			i = stack.back();
			stack.pop_back();

		}

		// Jump
		else if (opcode == 0xC8) {
			if (i + 5 < buffer.size()) {
				// Not entirely sure how this works yet.

				// const u8 p[3] = { buffer[i + 2], buffer[i + 3], buffer[i + 4] };

				// u24 pointer = JAudio::Core::swapEndian<u24>(p);

				// i += 4;

				// stack.push_back(i);
				// i = (size_t)pointer;

				i += 5;

				// std::cout << "\tIgnoring Jump: " << PTR << i << std::endl;

			} else { break; }

		} else if (opcode == 0xFD) {
			if (i + 3 < buffer.size()) {
				m_tempoMap[globalClock] = (int)buffer[i + 2];

				std::cout << PTR << globalClock << ": " << std::dec << m_tempoMap[globalClock] << std::hex << std::endl;

				i += 3;
			} else { break; }

		} else if (opcode == 0xFE) {
			if (i + 3 < buffer.size()) {
				m_ppqn = (int)buffer[i + 2];

				std::cout << "PPQN: " << std::dec << m_ppqn << std::hex << std::endl;

				i += 3;
			} else { break; }
		}

		// Other stuff
		// NoOp
		else if (opcode == 0x00) { i++; }

		// 1 param
		else if (opcode == 0xF4) { if (i + 2 < buffer.size()) { i += 2; } }

		// 2 params
		else if (
			opcode == 0x98 ||
			opcode == 0xA0 ||
			opcode == 0xA4 ||
			opcode == 0xA6 ||
			opcode == 0xA7 ||
			opcode == 0xCB ||
			opcode == 0xCC ||
			opcode == 0xD2 ||
			opcode == 0xDA ||
			opcode == 0xE6 ||
			opcode == 0xE7
		) { if (i + 3 < buffer.size()) { i += 3; } }

		// 3 params
		else if (
			opcode == 0x9A ||
			opcode == 0x9C ||
			opcode == 0xAC ||
			opcode == 0xAD
		) { if (i + 4 < buffer.size()) { i += 4; } }
		
		// 4 params
		else if (opcode == 0x9E) { if (i + 5 < buffer.size()) { i += 5; } }

		// Completely unknown or 0x00
		else {
			std::cout << "[WARNING]: Came across unhandled opcode `" << OPC << (int)opcode << " at " << PTR << (int)i << std::endl;
			break;
		}
	}

	file.close();

	std::cout << std::dec << std::setfill(' ');
	std::cout << "Finished Parsing Successfully!" << std::endl;
	return true;
	std::cout << "Something has gone horribly wrong..." << std::endl;
}