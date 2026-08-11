/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : JAudio/Core/endian.hpp
 * 	Project : libJAudio
 * 
 ********************************************************************************************************************/

#pragma once

#include <JAudio/Core/Types>

namespace JAudio::Core {

	template<JAudio::Core::integral T>
	inline T swapEndian(const u8 *data) {
		T out = 0;

		for (int i = 0; i < sizeof(T); i++) {
			out = (static_cast<T>(out) << 8) | static_cast<T>(data[i]);
		}

		return out;
	}

	template<JAudio::Core::integral T>
	inline T swapEndian(T data) {
		T out = 0;

		for (int i = 0; i < sizeof(T); i++) {
			out = (static_cast<T>(out) << 8 | static_cast<T>(data & 0xFF));
			data >>= 8;
		}

		return out;
	}

};