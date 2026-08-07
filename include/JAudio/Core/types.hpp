/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : JAudio/core/types.hpp
 * 	Project : libJAudio
 * 
 ********************************************************************************************************************/
#pragma once

#include <cstdint>

namespace JAudio::Core {

	class alignas(1) uint24_t {
		private:
			uint8_t data[3];
		
		public:
			uint24_t() = default;

			constexpr uint24_t(uint32_t val)
				: data {
					static_cast<uint8_t>(val >>  0),
					static_cast<uint8_t>(val >>  8),
					static_cast<uint8_t>(val >> 16)
				}
			{}

			constexpr operator uint32_t() const {
				return
					static_cast<uint32_t>(data[0] <<  0) |
					static_cast<uint32_t>(data[1] <<  8) |
					static_cast<uint32_t>(data[2] << 16);
			}

	};

	class alignas(1) int24_t {
		private:
			uint8_t data[2];
			int8_t  sign;
		
		public:
			int24_t() = default;

			constexpr int24_t(int32_t val) :
				data {
					static_cast<uint8_t>(val >> 0),
					static_cast<uint8_t>(val >> 8)
				},
				sign (static_cast< int8_t>(
					((val >> 16) & 0x7F) |
					((val >> 24) & 0x80)
				))
			{ }

			constexpr operator int32_t() const {
				return
					static_cast<int32_t>(data[0] <<  0) |
					static_cast<int32_t>(data[1] <<  8) |
					static_cast<int32_t>(sign    << 16);
			}
			
	};

	typedef uint8_t   u8;
	typedef uint16_t u16;
	typedef uint32_t u32;
	typedef uint24_t u24;
	typedef uint64_t u64;

	typedef int8_t   s8;
	typedef int16_t s16;
	typedef int32_t s32;
	typedef int24_t s24;
	typedef int64_t s64;

};

using JAudio::Core::u8;
using JAudio::Core::u16;
using JAudio::Core::u24;
using JAudio::Core::u32;
using JAudio::Core::u64;

using JAudio::Core::s8;
using JAudio::Core::s16;
using JAudio::Core::s24;
using JAudio::Core::s32;
using JAudio::Core::s64;