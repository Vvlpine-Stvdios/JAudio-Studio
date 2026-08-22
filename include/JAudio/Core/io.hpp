/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/20
 * 	File    : JAudio/Core/endian.hpp
 * 	Project : libJAudio
 * 
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#include <JAudio/Core/Types>
#include <JAudio/Core/Endian>

#include <vector>

namespace JAudio::Core::IO {

	template<JAudio::Core::integral T>
	inline T read(const std::vector<u8> &data, size_t offset, JAudio::Core::ENDIAN endian = JAudio::Core::ENDIAN::BIG) {
		T out = 0;

		// Big    endian = 0 => false
		// Little endian = 1 => true
		for (size_t i = 0; i < sizeof(T); i++) {
			out = endian
				? (static_cast<T>(data[offset + i]) << 8) | static_cast<T>(       out      )
				: (static_cast<T>(       out      ) << 8) | static_cast<T>(data[offset + i]);
		}

		return out;
	}

	inline void read(const std::vector<u8> &data, size_t offset, int numBytes, u8 *out) {
		if (offset + numBytes > data.size()) {
			out = std::vector<u8>(data.begin() + offset, data.end()).data();
		} else {
			out = std::vector<u8>(data.begin() + offset, data.begin() + offset + numBytes).data();
		}
	}

	template<JAudio::Core::integral T>
	inline void write(std::vector<u8> &buffer, T data, JAudio::Core::ENDIAN endian = JAudio::Core::ENDIAN::LITTLE) {
		if (!endian) {
			data = JAudio::Core::swapEndian<T>(data);
		}

		for (int i = 0; i < sizeof(T); i++) {
			buffer.push_back((data & 0xFF));
			data >>= 8;
		}
	}

	inline void write(std::vector<u8> &buffer, const std::vector<u8> &data) {
		buffer.insert(buffer.end(), data.begin(), data.end());
	}

};