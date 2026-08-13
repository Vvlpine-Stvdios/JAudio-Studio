/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : config.hpp
 * 	Project : JAudio Studio
 * 
 *	JAudio Studio - Uses libJAudio to parse and convert JAudio files into standard formats, and displays that data.
 * 	Copyright (C) 2026 Vulpine Studios
 * 
 ********************************************************************************************************************/

#pragma once

#include <string>

#define JAUDIO_STUDIO_VERSION_MAJOR 1
#define JAUDIO_STUDIO_VERSION_MINOR 2
#define JAUDIO_STUDIO_VERSION_PATCH 0
#define JAUDIO_STUDIO_VERSION_STR std::to_string(LIBJAUDIO_VERSION_MAJOR) + "." + std::to_string(LIBJAUDIO_VERSION_MINOR) + "." + std::to_string(LIBJAUDIO_VERSION_PATCH)
#define JAUDIO_STUDIO_COPYRIGHT "Copyright (C) Vulpine Studios"