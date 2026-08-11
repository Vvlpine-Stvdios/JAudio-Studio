#*******************************************************************************************************************
# 
# 	Author  : Burke Weston
# 	Date    : 2026/08/06
# 	File    : CMakeLists.txt
# 	Project : JAudio Studio
# 
#*******************************************************************************************************************#

## --- SETUP --- ##
cmake_minimum_required(VERSION 3.16)
project(JAudioStudio VERSION 1.0 LANGUAGES CXX)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Setup C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Config
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Concurrent)

## --- JAUDIO LIBRARY --- ##
add_library(
	JAudio SHARED
	# JAudio/AAF
	JAudio/Core/types.cpp
	JAudio/BMS/Parser.cpp
	JAudio/BMS/MIDIExporter.cpp
	include/JAudio/Core/endian.hpp
	include/JAudio/Core/types.hpp
	include/JAudio/BMS/commands.hpp
	include/JAudio/BMS/parser.hpp
	include/JAudio/BMS/midiexporter.hpp
)

target_include_directories(JAudio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

## --- APPLICATION --- ##
add_executable(
	JAudioStudio
	src/main.cpp
	src/MainWindow.cpp
	src/DropZone.cpp
	src/PianoRoll.cpp
	src/BulkConverterDialog.cpp
	include/mainwindow.hpp
	include/dropzone.hpp
	include/pianoroll.hpp
	include/bulkconverterdialog.hpp
)

set_target_properties(JAudioStudio PROPERTIES MACOSX_BUNDLE FALSE)

# Link
target_link_libraries(
	JAudioStudio PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
	Qt6::Concurrent
	JAudio
)