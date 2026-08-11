/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/11
 * 	File    : JAudio/Core/types.hpp
 * 	Project : libJAudio
 * 
 ********************************************************************************************************************/

#include <JAudio/Core/Types>

constexpr JAudio::Core::uint24_t operator+(JAudio::Core::uint24_t lhs, JAudio::Core::uint24_t rhs) { return JAudio::Core::uint24_t(static_cast<uint32_t>(lhs) + static_cast<uint32_t>(rhs)); }
constexpr JAudio::Core::uint24_t operator-(JAudio::Core::uint24_t lhs, JAudio::Core::uint24_t rhs) { return JAudio::Core::uint24_t(static_cast<uint32_t>(lhs) - static_cast<uint32_t>(rhs)); }
constexpr JAudio::Core::uint24_t operator*(JAudio::Core::uint24_t lhs, JAudio::Core::uint24_t rhs) { return JAudio::Core::uint24_t(static_cast<uint32_t>(lhs) * static_cast<uint32_t>(rhs)); }
constexpr JAudio::Core::uint24_t operator/(JAudio::Core::uint24_t lhs, JAudio::Core::uint24_t rhs) { return JAudio::Core::uint24_t(static_cast<uint32_t>(lhs) / static_cast<uint32_t>(rhs)); }

constexpr JAudio::Core::uint24_t operator<<(JAudio::Core::uint24_t lhs, int rhs) { return JAudio::Core::uint24_t(static_cast<uint32_t>(lhs) << rhs); }
constexpr JAudio::Core::uint24_t operator>>(JAudio::Core::uint24_t lhs, int rhs) { return JAudio::Core::uint24_t(static_cast<uint32_t>(lhs) >> rhs); }