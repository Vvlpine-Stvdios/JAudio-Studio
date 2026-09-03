<h1 style="text-align: center;">JaiInit.aaf</h1>

!!! warning "Warning!"
	This site is still under construction, and not all data is known (to me, at least).
	If you find the documentation in this faulty, misguiding, unclear, or straight missing information, please submit a [documentation issue](https://github.com/Vvlpine-Stvdios/JAudio-Studio/issues/new?template=documentation_correction.yml)!
	THIS SITE IS CURRENTLY DOCUMENTING JAUDIO VERSION ***<u>1</u>***. If you want information regarding JAudio Version 2, please go to [Luma’s Workshop](https://www.lumasworkshop.com/wiki/Main_Page).

## General Overview
The JaiInit.aaf file is main information file pertaining to all things streamed audio, including file names and headers for both the [AW](./AW.md) (instrument bank) files and the [AFC](./AFC.md) (streamed audio) files, as well as what goes with which instrument, &c.

## Types & Terminology
Throughout this I will refer to different types and user certain terms that. If you come across any term you do not know, check the [Types and Terminology Page](../types-and-terminology.md).

## Generic Headers
Each major section of this file is delineated by a generic header structure as follows:

|  Type   |  Name   |                  Description                                 |
| ------- | ------- | ------------------------------------------------------------ |
| `u32`   | ID      | Each ID corresponds to a different [section](#section-table) |
| `u32 *` | Pointer | The location where this section is to be found               |
| `u32`   | Size    | The size of this section                                     |

| ID | <span id="section-table">Section</section> |
| -: | ------------------------------------------ |
|  0 |  EOF[^1]                                   |
|  1 | [BST](#bst)                                |
|  2 | [IBNKs](#ibnk)                             |
|  3 | [WSYSs](#wsys)                             |
|  4 |  BSC[^1]                                   |
|  5 | [BSM](#bsm)                                |
|  6 | [SCL](#scl)[^1]                            |
|  7 | [SSC](#ssc)[^1]                            |

## BST
The BST (Binary Sound Table) is a container for all of the parameters for music and sound effects. Exactly how it functions, I am not entire sure yet.

## IBNK
The IBNK (Instrument Bank) is exactly what it sounds like; a giant container for all of the instrument data for some collection of instruments.

|   Type    |       Name       |   Description   |
| --------- | -----------------| --------------- |
| `char[4]` | Magic            | ‘IBNK’ in ASCII |
| `u32`     | Size             |                 |
| `u32`     | Bank ID          |                 |
| `u32`     | Bank Version[^1] |                 |

### BANK
IBNKs are always followed by 16 bytes of padding before the BANK structure:

|     Type     |                    Name                    |                                                                      Description                                                                      |
| ------------ | ------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------- |
| `char[4]`    | Magic                                      | ‘BANK’ in ASCII                                                                                                                                       |
| `(u32 *)[n]` | [INST](#inst) / [PER2](#per2) Pointers[^2] | There are at most 243 of these. If there is a specific point where the INST pointers stop and the PER2 pointers begin, I do not know. Ignored when 0. |

### INST
Each INST (Instrument) structure contains basic information about the instrument and where to find its stuff.

|     Type     |                        Name                        |                    Description                    |
| ------------ | -------------------------------------------------- | ------------------------------------------------- |
| `char[4]`    | Magic                                              | ‘INST’ in ASCII                                   |
| `u32`        | Unknown                                            |                                                   |
| `f32`        | Global Volume                                      | Multiplier (%)                                    |
| `f32`        | Global Pitch                                       | Multiplier (%)                                    |
| `u32 *`      | [Oscillator](#oscillator) Pointer                  |                                                   |
| `u32[5]`     | Unknown                                            | Likely padding                                    |
| `u32`        | Number of Keyboard Sections                        | `n`                                               |
| `(u32 *)[n]` | [Keyboard Section](#keyboard-section) Pointers[^2] | One pointer for each of the `n` keyboard sections |

#### Oscillator
Oscillators are used for modulation and ADSR (or some form of that).

|  Type   |                 Name                 |                                               Description                                               |
| ------- | ------------------------------------ | ------------------------------------------------------------------------------------------------------- |
| `u32`   | Target                               | What the oscillator controls: <ol start="0"><li>Volume</li><li>Pitch</li><li>Pan</li></ol>              |
| `f32`   | Rate                                 | Envelope speed multiplier                                                                               |
| `u32 *` | [Attack](#vector-table) Pointer[^2]  | The pointer to the [vector table](#vector-table) holding the information for the attack/sustain/decay   |
| `u32 *` | [Release](#vector-table) Pointer[^2] | The pointer to the [vector table](#vector-table) holding the information for the release                |
| `f32`   | Width                                | Modulation depth                                                                                        |
| `u32`   | Vertex Count                         | Populated by engine at run time[^1]                                                                     |
| `u8[8]` | Padding                              |                                                                                                         |

#### Vector Table
These are simply an (x, y) plot of points relating to envelope curves.

| Type  | Name  |                                                                                                     Description                                                                                                      |
| ----- | ----- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `u16` | Mode  | The type of curve / command:<ol start="0" style="margin-bottom: 0;"><li>Linear</li><li>Square</li><li>Root</li><li>SampleCell</li></ol><ol type="A" start="5" style="margin-top: 0;"><li>Hold</li><li>Stop</li></ol> |
| `u16` | Time  | Measured in ticks                                                                                                                                                                                                    |
| `u16` | Value |                                                                                                                                                                                                                      |

#### Keyboard Section
These provide information regarding which sample(s) to use for a given range of keys. For those experienced with samplers, you might have heard these called *zones*.

|     Type     |                       Name                       |                                Description                                |
| ------------ | ------------------------------------------------ | ------------------------------------------------------------------------- |
| `u8`         | Highest Key                                      | Defines the highest key of this zone. 0x00 is C-1, 0x3C is C4, 0x7F is G9 |
| `u8[3]`      | Padding                                          |                                                                           |
| `u32`        | Velocity Region Count                            | `n`                                                                       |
| `(u32 *)[n]` | [Velocity Region](#velocity-region) Pointers[^2] | One pointer for each of the `n` velocity regions.                         |

#### Velocity Region
These provide information regarding which sample to use for a given range of velocities.

|  Type   |       Name       |                             Description                              |
| ------- | ---------------- | -------------------------------------------------------------------- |
| `u8`    | Highest Velocity | Defines the highest velocity of this region. 0x00 is 0, 0x7F is 127. |
| `u8[3]` | Padding          |                                                                      |
| `u16`   | Bank ID          | Assumed based off of v2 documentation; is unverified.                |
| `u16`   | Wave ID          | Assumed based off of v2 documentation; is unverified.                |
| `f32`   | Volume           |                                                                      |
| `f32`   | Pitch            |                                                                      |

### PER2
PER2s (Percussion [no idea what the 2’s for]) work the same way as INST structures, except they remove unneeded information.

|      Type      |                   Name                   |                    Description                    |
| -------------- | ---------------------------------------- | ------------------------------------------------- |
| `char[4]`      | Magic                                    | ‘PER2’ in ASCII                                   |
| `(u32 *)[128]` | [Drum Region](#drum-region) Pointers[^2] | One for each key of the keyboard. Ignored when 0. |

#### Drum Region
These are quite similar to INST structures, but since the PER2 allocates one pointer per key, keyboard sections are forwent in favor of directly pointing to velocity regions.

|     Type     |                       Name                       |                   Description                    |
| ------------ | ------------------------------------------------ | ------------------------------------------------ |
| `f32`        | Global Volume                                    |                                                  |
| `f32`        | Global Pitch                                     |                                                  |
| `u32[2]`     | Unknown                                          | Might be FX                                      |
| `u32`        | Velocity Region Count                            | `n`                                              |
| `(u32 *)[n]` | [Velocity Region](#velocity-region) Pointers[^2] | One pointer for each of the `n` velocity regions |

## WSYS
The WSYS (Wave System) contains all of the header and reference data for samples. Each begins with a simple header that points to an amalgamation of other things.

|   Type    |           Name            |   Description   |
| --------- | ------------------------- | --------------- |
| `char[4]` | Magic                     | ‘WSYS’ in ASCII |
| `u32`     | Size                      |                 |
| `u32`     | ID                        |                 |
| `u32`     | Wave Count                |                 |
| `u32 *`   | [WINF](#winf) Pointer[^2] |                 |
| `u32 *`   | [WBCT](#wbct) Pointer[^2] |                 |

### WINF
The WINF (Wave Info) just contains the pointers to [Wave Groups](#wave-group).

|     Type     |                  Name                  |                 Description                  |
| ------------ | -------------------------------------- | -------------------------------------------- |
| `char[4]`    | Magic                                  | ‘WINF’ in ASCII                              |
| `u32`        | Wave Group Count                       | `n`                                          |
| `(u32 *)[n]` | [Wave Group](#wave-group) Pointers[^2] | One pointer for each of the `n` Wave Groups. |

#### Wave Group
Wave Groups contain the name of the archive file that contains the sample information and pointers to the wave headers.

|     Type     |            Name            |                               Description                               |
| ------------ | -------------------------- | ----------------------------------------------------------------------- |
| `char[112]`  | Archive Name               | 112 bytes are always reserved for the file name. Unused bytes are null. |
| `u32`        | Wave Count                 | `n`                                                                     |
| `(u32 *)[n]` | [WAVE](#wave) Pointers[^2] | One pointer for each of the `n` WAVEs                                   |

#### WAVE
WAVEs contain the specific sample information such as format and placement in the [AW](./AW.md) archive files.

|  Type   |       Name        |                                                 Description                                                  |
| ------- | ----------------- | ------------------------------------------------------------------------------------------------------------ |
| `u8`    | Unknown           | Always 0xFF                                                                                                  |
| `u8`    | Format            | Defines the format of the data: <ol start="0"><li>ADPCM4</li><li>ADPCM2</li><li>PCM8</li><li>PCM16</li></ol> |
| `u8`    | Base Key          | 0x00 is C-1, 0x3C is C4, 0x7F is G9                                                                          |
| `u8`    | Unknown           | Always 0x00                                                                                                  |
| `f32`   | Sample Rate       | Why is it a float? Good question.                                                                            |
| `u32 *` | AW Offset Start   | Pointer into the AW file to where this sound starts                                                          |
| `u32`   | AW Length         | In bytes                                                                                                     |
| `sb32`  | Loop?             |                                                                                                              |
| `u32`   | Loop Start Sample |                                                                                                              |
| `u32`   | Loop End Sample   |                                                                                                              |
| `u32`   | Sample Count      |                                                                                                              |
| `s16`   | Last              | ADPCM uses the last two samples to predict the next one. This is the sample “previous” the first.            |
| `s16`   | Penult            | This is the sample “previous” the last (see above) sample                                                    |

### WBCT
The WBCT (Wave Bank Control Table) structure contains pointers to [SCNEs](#scne). That’s about it.

|     Type     |          Name          |              Description              |
| ------------ | ---------------------- | ------------------------------------- |
| `char[4]`    | Magic                  | ‘WBCT’ in ASCII                       |
| `sb32`       | Unknown                | Always true                           |
| `u32`        | SCNE Count             | `n`                                   |
| `(u32 *)[n]` | [SCNE](#scne) Pointers | One pointer for each of the `n` SCNEs |

#### SCNE
The SCNE (Scene) structure points to three tables, only one of which is actually used.

|   Type    |                Name                 |                                    Description                                     |
| --------- | ----------------------------------- | ---------------------------------------------------------------------------------- |
| `char[4]` | Magic                               | ‘SCNE’ in ASCII                                                                    |
| `u32[2]`  | Unknown                             | May be used to point to other things, but both of these are always 0 in Wind Waker |
| `u32 *`   | [C-DF](#c-df-c-ex-and-c-st) Pointer |                                                                                    |
| `u32 *`   | [C-EX](#c-df-c-ex-and-c-st) Pointer |                                                                                    |
| `u32 *`   | [C-ST](#c-df-c-ex-and-c-st) Pointer |                                                                                    |

#### C-DF, C-EX, and C-ST
The C-DF (Chunk – Data Format) is another pointer container giving information about the whereabouts of a few bits of metadata.

|     Type     |            Name            |               Description               |
| ------------ | -------------------------- | --------------------------------------- |
| `char[4]`    | Magic                      | ‘C-DF’ in ASCII                         |
| `u32`        | WAVEID Count               | `n`                                     |
| `(u32 *)[n]` | [WAVEID](#waveid) Pointers | One pointer for each of the `n` WAVEIDs |

The C-EX (Chunk — Extra) and C-ST (Chunk — String Table) are completely unused in Wind Waker and can be ignored. They follow the same structure as the C-DF, but everything is zero.

#### WAVEID
The WAVEID appears to contain some metadata for each [WAVE](#wave), though seemingly only the, well, wave ID.
According to [Luma’s Workshop](https://www.lumasworkshop.com/wiki/WSYS#WAVEID), JAudio v2 uses two 16-bit integers to represent the Wave Group ID and the WAVE ID;
however what would be the Wave Group ID (which is supposed to match the WSYS) is always `0x0000`.
Therefore, I am simply calling the whole 32-bits the Wave ID, but keep in mind it might be different.

|   Type    |  Name   |  Description   |
| --------- | ------- | -------------- |
| `u32`     | Wave ID | See note above |
| `u32[12]` | Unknown | Always 0       |
| `sb32`    | Unknown | Always true    |

## BSM
The BSM (Binary Stream Map) is a structure that holds all the header information for the streamed [AFC](./AFC.md) files.
Each entry contains the file name and a copy of its header.

|   Type   |  Name   |                                 Description                                 |
| -------- | ------- | --------------------------------------------------------------------------- |
| `u32[4]` | Unknown | This is 0x00000006 00000000 00000000 00000000. I do not know what it means. |

Each entry goes as follows:

|    Type    |       Name        |                                                Description                                                |
| ---------- | ----------------- | --------------------------------------------------------------------------------------------------------- |
| `char[16]` | File Name         | 16 byte are always reserved. Unused bytes are null.                                                       |
| `u32`      | Size              |                                                                                                           |
| `u32`      | Sample Count      |                                                                                                           |
| `u16`      | Sample Rate       | A much more reasonable data type for this application                                                     |
| `u16`      | Bits per Sample   | This is unverified, but as every file is ADPCM4 and this is always 4, I am going with it.                 |
| `u16`      | Samples per Frame | Also a guess, but it’s always 16, and all of the files have 16 samples per frame.                         |
| `u16`      | Unknown           | Either 0x1E (30) or 0x3C (60)                                                                             |
| `ub32`     | Loop?             |                                                                                                           |
| `u32`      | Loop Start        | Measured in Samples. The end is assumed to be the sample count—the end of the file.                       |
| `u16`      | Penult            | These are also my own assumption, as the penult and last are assumed to be zero anyways, which there are. |
| `u16`      | Last              | See above.                                                                                                |
| `u32`      | Padding           |                                                                                                           |

## SCL[^1]
Supposedly this penultimate section is the SCL (Sequence Category Limits).
According to AI, this holds specifically maximum polyphony for 16 different categories.

|     Type     |     Name      |                                                           Description                                                           |
| ------------ | ------------- | ------------------------------------------------------------------------------------------------------------------------------- |
| `u32`        | Pointer Count | This is `0x02`                                                                                                                  |
| `(u32 *)[n]` | Pointers[^2]  | There are two pointers here. Each points to an array of 16 `0x04`’s. According to AI this means a max of 4 voices per category. |

## SSC[^1]
Supposedly this last section is the SSC (Stream/Scene Configuration). This is supposed to tell the Game Cube basic information about the streams, such as maximum number of concurrent streams, and how to transition between them.
I do not know exactly how this is laid out yet.

---

[^1]: <em>This is not verified; it was simply what an AI told me an it may or may not be accurate.</em>
[^2]: <em>All pointers inside major structures defined at the beginning are offset relative to that structure’s header.</em>

---