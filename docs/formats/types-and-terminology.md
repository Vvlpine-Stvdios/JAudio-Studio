<h1 style="text-align: center;">Types and Terminology</h1>

!!! warning "Warning!"
	This site is still under construction, and not all data is known (to me, at least).
	If you find the documentation in this faulty, misguiding, unclear, or straight missing information, please submit a [documentation issue](https://github.com/Vvlpine-Stvdios/JAudio-Studio/issues/new?template=documentation_correction.yml)!
	THIS SITE IS CURRENTLY DOCUMENTING JAUDIO VERSION ***<u>1</u>***. If you want information regarding JAudio Version 2, please go to [Luma’s Workshop](https://www.lumasworkshop.com/wiki/Main_Page).

Here are some general types and terminology I refer to througout this documentation. To make this as clear as possible, I will list them here with some definitions:

|   Type   |               Definition                |            What it can store            |                                                                                Usage                                                                                 |
| -------- | --------------------------------------- | --------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Nibble` | Half of a byte; 4 bits                  | n ∈ [0, 15] ∋ ℤ                         | Used to store sample data in ADPCM4. Nibbles greater or equal to 8 (high bit true) have 16 subtracted from them to make negative, making the effective range [-8, 7] |
| `char`   | One byte (8-bits)                       | ASCII characters                        | Used for magic strings.                                                                                                                                              |
| `u8`     | An Unsigned, 8-bit (1-byte) Integer     | n ∈ [0, 255] ∋ ℤ                        | Often used to store opcodes, or used to save storage space.                                                                                                          |
| `u16`    | An Unsigned, 16-bit (2-byte) Integer    | n ∈ [0, 65,535] ∋ ℤ                     | Rarely used, most often used to save on space.                                                                                                                       |
| `u24`    | An Unsigned, 24-bit (3-byte) Integer    | n ∈ [0, 16,777,215] ∋ ℤ                 | Extremely rare. Often used to store pointers in BMS files.                                                                                                           |
| `u32`    | An Unsigned, 32-bit (4-byte) Integer    | n ∈ [0, 4,294,967,295] ∋ ℤ              | Most common. Used to store pointers and any generic count.                                                                                                           |
| `s8`     | A Signed, 8-bit (1-byte) Integer        | n ∈ [-128, 127] ∋ ℤ                     | Extremely rarely used, as most things don’t need negative numbers.                                                                                                   |
| `s16`    | A Signed, 16-bit (2-byte) Integer       | n ∈ [-32,768, 32,767] ∋ ℤ               | "                                                                                                                                                                    |
| `s24`    | A Signed, 24-bit (3-byte) Integer       | n ∈ [-8,388,608, 8,388,607] ∋ ℤ         | "                                                                                                                                                                    |
| `s32`    | A Signed, 32-bit (4-byte) Integer       | n ∈ [-2,147,483,648, 2,147,483,647] ∋ ℤ | "                                                                                                                                                                    |
| `f16`    | A 32-bit (4-byte) Floating-Point Number | Decimals with ~3 digits of precision    | Used in BMS files to store floating point numbers.                                                                                                                   |
| `f32`    | A 32-bit (4-byte) Floating-Point Number | Decimals with ~7 digits of precision    | Default floating point number. Often used for multipliers and percents.                                                                                              |
| `ub8`    | An Unsigned, 8-bit (1-byte) Boolean     | 0 or 1                                  | 0 is false, 1 is true.                                                                                                                                               |
| `ub16`   | An Unsigned, 16-bit (2-byte) Boolean    | 0 or 1                                  | "                                                                                                                                                                    |
| `ub32`   | An Unsigned, 32-bit (4-byte) Boolean    | 0 or 1                                  | "                                                                                                                                                                    |
| `sb8`    | A Signed, 8-bit (1-byte) Boolean        | 0 or -1                                 | Because we’re special, 0 is false, -1 is true.                                                                                                                       |
| `sb16`   | A Signed, 16-bit (2-byte) Boolean       | 0 or -1                                 | "                                                                                                                                                                    |
| `sb32`   | A Signed, 32-bit (4-byte) Boolean       | 0 or -1                                 | "                                                                                                                                                                    |

You will also come across this notation: `u32[n]`. The brackets specify an *array*, or a collection of whatever datatype n times. Think of smarties; those could be called arrays of candy.

Another common notation is for pointers: `u32 *`. The star denotes that this value stores a location in memory. You may see `(u32 *)[n]`; this is just an array of pointers.

## Terminology
You will see "Magic" a lot when reading this documentation. This refers to how you put a seemingly random 32-bit number in a location and that tells programs information. Magics are typically 4 characters long and saved using ASCII (basically this just associates numbers with letters).

Assume decimal with numbers. The prefix `0x` denotes **hexadecimal** (base 16 [0–9, A–F]); the prefix `0o` denotes **octal** (base 8 [0–8]); the prefix `0b` denotes **binary** (base 2 [0–1]).

All note names will use the Roland scale, where Middle C is C4.

## Streamed versus Sequenced Music; What’s the difference?
There are two main ways to store music information: ‘streamed’ and ‘sequenced.’

**Streamed** audio stores the actual audio wave data and is what you are most likely to come across today.
MP3’s, WAV’s and anything you play off your music app of choice is *streamed audio*.

On the other hand, **sequenced** audio stores the notes and modulation data of a song.
The most common form of sequenced audio today is the MIDI file.
These files do not contain any actual audio—rather they tell another application at what pitch, time, velocity, &c. to play some sample.

## What the heck is ADPCM? (or PCM, for that matter?)
PCM stands for Pulse Code Modulation, and is a way of storing [streamed](#streamed-versus-sequenced-music-whats-the-difference) audio.
PCM records the amplitude of a wave form at regular intervals (the sample rate) and saves those as values into a file.
This is why WAV files are so large; WAV files are typically PCM16, meaning they take 16 bits to store a single point, and they store every single point, typically at ~44.1kHz.
There is also PCM8 which uses half the size.

DPCM stands for Differential Pulse Code Modulation, and is a much more efficient way of storing audio data.
DPCM predicts the next sample based on the previous sample and some pre-defined coefficient and only stores the error from that predicted value to the actual value to the file.

ADPCM stands for Adaptive Differential Pulse Code Modulation, and is even more versatile than DPCM.
This uses a selection of various coefficient pairs and the previous *two* samples to predict the next one, once again storing the error.
As such, ADPCM stores audio in *frames;* in the case of AFC files, 18-nibble (9-byte) chunks composed of a header byte, specifying the coefficients and scale, and 16 samples.
Each frame decides which coefficients it will use and at what scale.

JAudio can use PCM8 (8 bits per sample), PCM16 (16 bits per sample), ADPCM4 (4 bits per sample) and ADPCM2 (2 bits per sample).