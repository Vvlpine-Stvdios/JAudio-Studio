JAudio Studio is a free, open source JAudio file editor. It is currently in development, so "editor" is perhaps a bit generous. Currently it only supports Wind Waker BMS files with MIDI export.
libJAudio is the library that does the heavy-lifting, parsing and converting files. It is also free and open-source.
# Features
- View BMS sequence files on a piano roll
- View automation data of BMS files
- Rename tracks
- Mark tracks as percussion (export on MIDI channel 10)
- Export BMS files as MIDI
- Bulk export BMS files.

# Goals
- Parse entire Wind Waker JAudio folder
- Convert bank and streamed files to WAV
- Export bank files as SF2
- Display folder structure to easily access files
- Add tabs to edit multiple files at once
- Make it look good

# License
JAudio Studio and libJAudio are both licensed under GPL version 3.0. See the [LICENSE](license) for details.

# Building
The build system for JAudio Studio and libJAudio is CMake with C++23. JAudio Studio uses the Qt graphics framework. Make sure both are properly installed.
To get the repository, run the following:
```bash
git clone https://github.com/Vvlpine-Stvdios/JAudio-Studio.git
cd JAudio-Studio
```
To build, run the following:
```bash
cmake -S . -B cmake --config Release
```
Once that has finished, run the following:
```bash
cmake --build cmake
```
To run, run the following:
```bash
cmake --build cmake --target run
```
