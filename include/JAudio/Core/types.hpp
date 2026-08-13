/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : JAudio/Core/types.hpp
 * 	Project : libJAudio
 * 
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vulpine Studios
 * 
 ********************************************************************************************************************/
#pragma once

#include <type_traits>
#include <cstdint>

namespace JAudio::Core {

	class alignas(1) uint24_t {
		private:
			uint8_t data[3];
		
			constexpr uint32_t to_u32() const {
				return
					 static_cast<uint32_t>(data[0])        |
					(static_cast<uint32_t>(data[1]) <<  8) |
					(static_cast<uint32_t>(data[2]) << 16);
			}

			constexpr void from_u32(uint32_t value) {
				data[0] = static_cast<uint8_t>( value        & 0xFF);
				data[1] = static_cast<uint8_t>((value >> 8)  & 0xFF);
				data[2] = static_cast<uint8_t>((value >> 16) & 0xFF);
			}

		public:
			uint24_t() = default;

			constexpr uint24_t(uint32_t value)
				: data {
					static_cast<uint8_t>(value >>  0),
					static_cast<uint8_t>(value >>  8),
					static_cast<uint8_t>(value >> 16)
				}
			{}

			// Implicit conversion to standard integer types
			constexpr          operator uint32_t() const { return to_u32(); }
			constexpr explicit operator uint64_t() const { return to_u32(); }

			// Assignment operators
			uint24_t &operator=(uint32_t value) {
				from_u32(value & 0xFFFFFF);

				return *this;
			}

			// Compound assignment & Arithmetic
			uint24_t &operator+=(const uint24_t &other) { from_u32((to_u32() + other.to_u32()) & 0xFFFFFF); return *this; }
			uint24_t &operator-=(const uint24_t &other) { from_u32((to_u32() - other.to_u32()) & 0xFFFFFF); return *this; }
			uint24_t &operator*=(const uint24_t &other) { from_u32((to_u32() * other.to_u32()) & 0xFFFFFF); return *this; }
			uint24_t &operator/=(const uint24_t &other) { from_u32((to_u32() / other.to_u32()) & 0xFFFFFF); return *this; }

			uint24_t &operator>>=(int value) { from_u32((to_u32() >> value) & 0xFFFFFF); return *this; }
			uint24_t &operator<<=(int value) { from_u32((to_u32() << value) & 0xFFFFFF); return *this; }

			// Increment / Decrement (postpend/prepend)
			uint24_t &operator++() { *this += 1; return *this; }
			uint24_t &operator--() { *this -= 1; return *this; }

			uint24_t operator++(int) { uint24_t tmp = *this; *this += 1; return tmp; }
			uint24_t operator--(int) { uint24_t tmp = *this; *this -= 1; return tmp; }
			
	};

	template<typename T>
	struct is_integral : std::is_integral<T> { };

	template<>
	struct is_integral<uint24_t> : std::true_type { };

	template <typename T>
	inline constexpr bool is_integral_v = is_integral<T>::value;

	template <typename T>
	concept integral = is_integral_v<T>;

	typedef uint8_t   u8;
	typedef uint16_t u16;
	typedef uint32_t u32;
	typedef uint24_t u24;
	typedef uint64_t u64;

	typedef int8_t   s8;
	typedef int16_t s16;
	typedef int32_t s32;
	typedef int64_t s64;

};

using JAudio::Core::u8;
using JAudio::Core::u16;
using JAudio::Core::u24;
using JAudio::Core::u32;
using JAudio::Core::u64;

using JAudio::Core::s8;
using JAudio::Core::s16;
using JAudio::Core::s32;
using JAudio::Core::s64;