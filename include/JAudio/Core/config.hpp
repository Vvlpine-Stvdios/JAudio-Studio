/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/12
 * 	File    : config.hpp
 * 	Project : libJAudio
 *  
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vvlpine Stvdios
 * 
 ********************************************************************************************************************/

#pragma once

#include <string>

#define LIBJAUDIO_VERSION_MAJOR 1
#define LIBJAUDIO_VERSION_MINOR 2
#define LIBJAUDIO_VERSION_PATCH 0
#define LIBJAUDIO_VERSION_STR std::to_string(LIBJAUDIO_VERSION_MAJOR) + "." + std::to_string(LIBJAUDIO_VERSION_MINOR) + "." + std::to_string(LIBJAUDIO_VERSION_PATCH)
#define LIBJAUDIO_COPYRIGHT "Copyright (C) Vvlpine Stvdios"