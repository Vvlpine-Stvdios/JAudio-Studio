/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : JAudio/core/endian.hpp
 * 	Project : libJAudio
 * 
 ********************************************************************************************************************/

#pragma once

#include <JAudio/Core/Types>
#include <iostream>
#include <type_traits>

namespace JAudio::Core {

	template<typename T>
	inline T from_big_endian(const u8 *data) {
		T out = 0;

		// Because u24s are weird, it's safer to just check and return it using u32's
		// if constexpr (std::is_same_v<T, u24>) {
		// 	return u24(
		// 		static_cast<u32>(data[0] << 16) |
		// 		static_cast<u32>(data[1] <<  8) |
		// 		static_cast<u32>(data[2] <<  0)
		// 	);
		// }

		for (int i = 0; i < sizeof(T); i++) {
			out = (static_cast<T>(out) << 8) | static_cast<T>(data[i]);
		}

		return out;
	}

};