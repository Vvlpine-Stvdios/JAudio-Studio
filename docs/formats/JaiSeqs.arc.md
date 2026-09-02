<h1 style="text-align: center;">JaiSeqs.arc</h1>

!!! warning "Warning!"
	This site is still under construction, and not all data is known (to me, at least).
	If you find the documentation in this faulty, misguiding, unclear, or straight missing information, please submit a [documentation issue](https://github.com/Vvlpine-Stvdios/JAudio-Studio/issues/new?template=documentation_correction.yml)!
	THIS SITE IS CURRENTLY DOCUMENTING JAUDIO VERSION ***<u>1</u>***. If you want information regarding JAudio Version 2, please go to [Luma’s Workshop](https://www.lumasworkshop.com/wiki/Main_Page).

## General Overview
The JaiSeqs.arc file is a large archive that stores all of the [BMS](./BMS.md) files. It begins with some general headers that provide locations, sizes, and names of the files. The file structure is the same across v1 and v2[^1], so most of this is straight from [Luma’s Workshop](https://www.lumasworkshop.com/wiki/RARC_(File_Format)).

!!! note "Note"
	This page is specifically about the JaiSeqs.arc file. There are other .arc files in the game that to not pertain to the music data.
	All of these structures should still apply, but keep in mind any direct references will likely not apply.

## RARC Header
This is the first thing in the file. It defines the type of file as well as general information like sizes and types of data.

|   Type    |                Name                 |                             Description                              |
| --------- | ----------------------------------- | -------------------------------------------------------------------- |
| `char[4]` | Magic                               | ‘RARC’ in ASCII                                                      |
| `u32`     | File Size                           |                                                                      |
| `u32 *`   | [Data Header](#data-header) Pointer | Always 0x20, as the data header is immediately after the RARC header |
| `u32 *`   | Data Pointer                        | Offset from the [Data Header](#data-header)                          |
| `u32`     | Data Size                           | Size of the entire file data                                         |
| `u32`     | MRAM Data Size                      | Size of specifically the MRAM (Main Random-Access Memory) files      |
| `u32`     | ARAM Data Size                      | Size of specifically the ARAM (Auxiliary Random-Access Memory) files |
| `u32`     | DVD Data Size                       | Size of specifically the DVD (Digital Versatile Disk[^2]) files      |

## Data Header
This is the second thing in the file, and it provides information about the files themselves, namely the number and locations of headers and names.

|  Type   |                        Name                        |    Description     |
| ------- | -------------------------------------------------- | ------------------ |
| `u32`   | Directory Node Count                               |                    | 
| `u32 *` | [Directory Nodes](#directory-node-section) Pointer | Offset from header | 
| `u32`   | File Node Count                                    |                    | 
| `u32 *` | [File Nodes](#file-node-section) Pointer           | Offset from header | 
| `u32`   | String Table Size                                  |                    | 
| `u32 *` | [String Table](#string-table) Pointer              | Offset from header | 
| `u16`   | Next Available File Index                          |                    | 
| `ub8`   | Sync IDs?                                          |                    | 
| `u8[5]` | Padding                                            |                    | 

## Directory Node Section
This contains a list of Directory Nodes. In the case of Wind Waker, there is only one in this archive.

### Directory Node

|   Type    |        Name        |                           Description                           |
| --------- | ------------------ | --------------------------------------------------------------- |
| `char[4]` | Name (Abbr.)       | The first four characters of the directory name in all caps[^3] |
| `u32 *`   | Name Pointer       | Offset from [String Table](#string-table)                       |
| `u16`     | Name Hash          |                                                                 |
| `u16`     | File Node Count    |                                                                 |
| `u32`     | File Nodes Pointer | Offset from end of section                                      |

## File Node Section
This contains a list of file nodes. These give information about where to find the file data and the file name in the file data and string table, respectively.

These nodes end with an extra 2 bytes that are always zero that are not listed in Luma’s Workshop.
I hesitate to call them *padding* because the structure is a clean 16 bytes without them, but for some reason two extra bytes of zero are tacked on the end making them an irregular size of 18 bytes.
If this is a difference between versions, it makes complete sense for these to be removed.

There are two different types of nodes: File Nodes and Subdirectory nodes. Each contain the same number of bytes, but they are used slightly differently.

### File Nodes

|  Type   |                Name                 |                                Description                                 |
| ------- | ----------------------------------- | -------------------------------------------------------------------------- |
| `u16`   | Index                               |                                                                            |
| `u16`   | Name Hash                           |                                                                            |
| `u8`    | [Node Attributes](#node-attributes) |                                                                            |
| `u24 *` | Name Pointer                        | Offset from [String Table](#string-table)                                  |
| `u32 *` | Data Pointer                        | Offset from the Data Section referenced in the [Data Header](#data-header) |
| `u32`   | Data Size                           |                                                                            |
| `u16`   | “Padding”                           |                                                                            |

### Subdirectory Nodes

|  Type   |                Name                 |                                                   Description                                                   |
| ------- | ----------------------------------- | --------------------------------------------------------------------------------------------------------------- |
| `u16`   | Index                               | Always 0xFFFF (Indicates Subdirectory)                                                                          |
| `u16`   | Name Hash                           |                                                                                                                 |
| `u8`    | [Node Attributes](#node-attributes) |                                                                                                                 |
| `u24 *` | Name Pointer                        | Offset from [String Table](#string-table).                                                                      |
| `s32`   | Directory Node Index                | Index into the [Directory Node Section](#directory-node-section). An index of -1 indicates top level container. |
| `u32`   | Directory Size                      |                                                                                                                 |
| `u16`   | “Padding”                           |                                                                                                                 |

### Node Attributes
The Node Attributes byte is a combination of 7 different bit flags indicating the type of file node and where it should be put.

|  Mask  |      Name       |                 Description                  |
| ------ | --------------- | -------------------------------------------- |
| `0x01` | File            |                                              | 
| `0x02` | Directory       |                                              | 
| `0x04` | Compressed      |                                              | 
| `0x10` | Preload to MRAM | File should be loaded into the main RAM      | 
| `0x20` | Preload to ARAM | File should be loaded into the auxiliary RAM | 
| `0x40` | Load from DVD   | File should be read off the disk             | 
| `0x80` | YAZ0 Compressed | File is specifically YAZ0 compressed         | 

## String Table
The String Table is just a giant array of null-terminated strings (this just means the string ends with the byte `00`, which is the NULL character. Programs look for this to know when a string ends).

|   Type    |    Name     |                      Description                       |
| --------- | ----------- | ------------------------------------------------------ |
| `char[n]` | String Name | A null-terminated string. (E.g., `62 6D 73 00`; “bms”) |
|    ...    | *&c.*       |                                                        |

---

[^1]: There are a couple differences in this documentation compared to Luma’s Workshop. In my efforts to parse this file, I found 16 bytes of padding between the Directory Node Section and the first File Node; and I found an extra two bytes tacked on to the end of the File Node structures.
[^2]: Yes, this normally means Digital *Video* Disk, but in this case we are not reading just video off of this disk, so another more fitting term is to use ‘Versatile’ instead of ‘Video.’
[^3]: Maybe. For this file, the one directory node is called “root,” but the string table name points to “bms.”