/********************************************************************************************************************
 * 
 * 	Author  : Burke Weston
 * 	Date    : 2026/08/06
 * 	File    : JAudio/BMS/commands.hpp
 * 	Project : libJAudio
 * 
 *	libJAudio - Parses and converts JAudio files into standard formats.
 * 	Copyright (C) 2026 Vulpine Studios
 * 
 ********************************************************************************************************************/
#pragma once

#include <JAudio/Core/Types>

#include <vector>

namespace JAudio::BMS {

	// class cmdGeneric {
	// 	public:
	// 		u8 opcode;
	// };

	// class cmdNoteOn      : virtual cmdGeneric { public: u8  voice;  u8 velocity;       };
	// class cmdWaitByte    : virtual cmdGeneric { public: u8  time;                      };
	// class cmdNoteOff     : virtual cmdGeneric {                                        };
	// class cmdWait        : virtual cmdGeneric { public: u16 time;                      };
	// class cmdDynamicsSet : virtual cmdGeneric { public: u8  target; u8 value;          };
	// class cmdPanSet      : virtual cmdGeneric { public: u8  arg0;   u8 value; u8 arg2; };

	// class cmdParamSet : virtual cmdGeneric {
	// 	public:
	// 		u8 target;
	// 		union {
	// 			u8  value, dvalue;
	// 			s16 value;
	// 		};
	// };

	// class cmdParamSetOverTime : virtual cmdGeneric {
	// 	public:
	// 		u8 target;
	// 		union {
	// 			u8  value, dvalue;
	// 			s16 value;
	// 		};
	// };

	// class cmdProgramChange     : virtual cmdGeneric { public: u8  target;  u8  value;   };
	// class cmdTrackPointer      : virtual cmdGeneric { public: u8  channel; u24 pointer; };
	// class cmdCall              : virtual cmdGeneric { public: u16 pointer;              };
	// class cmdCallF             : virtual cmdGeneric { public: u8  arg0;    u16 pointer; };
	// class cmdReturn            : virtual cmdGeneric { public: u8  arg0;                 };
	// class cmdJump              : virtual cmdGeneric { public: u8  arg0;    u24 pointer; };
	// class cmdDynamicTrackLabel : virtual cmdGeneric { public: u8  track;                };
	// class cmdModulationSet     : virtual cmdGeneric { public: u16 value;                };
	// class cmdTrackStart        : virtual cmdGeneric { public: u8  arg0;    u8  arg1;    };
	// class cmdTempoSet          : virtual cmdGeneric { public: u8  arg0;    u8  value;   };
	// class cmdPPQNSet           : virtual cmdGeneric { public: u8  arg0;    u8  value;   };
	// class cmdTrackEnd          : virtual cmdGeneric {                                   };

	// class cmdUnknown : virtual cmdGeneric {
	// 	public:
	// 		u8              opcode;
	// 		std::vector<u8> argv;
	// };

};