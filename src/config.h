//
// config.h
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2017-2020  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _config_h
#define _config_h

//#define USE_I2S				// use I2S device instead of PWM

#define SAMPLE_RATE		48000		// overall system clock

#if RASPPI >= 2
	#define VOICES_PER_CORE	6		// polyphonic voices per CPU core
#else
	#define VOICES_PER_CORE	4		// polyphonic voices (1 core only)
#endif

#define VELOCITY_DEFAULT	80		// for PC keyboard (max. 127)

#define PATCHES			10		// number of configurable patches, don't change

#define PARTITION		"emmc1-1"	// SD card partition to use

// configurable options
#define LAST_NOTE_PRIORITY			// last note priority polyphony

// color scheme
#if 1
	#define BACK_COLOR		0xEF7D
	#define FORE_COLOR		C_BLACK
	#define TITLE_BACK_COLOR	C_LIGHT_GRAY
	#define TITLE_FORE_COLOR	C_BLACK
	#define HEADER_BACK_COLOR	C_MEDIUM_AQUA_MARINE
	#define HEADER_FORE_COLOR	C_WHITE
	#define BUTTON_BACK_COLOR	0xEF7D
	#define BUTTON_HIGH_COLOR	C_MEDIUM_AQUA_MARINE
	#define VALUE_BACK_COLOR	C_LIGHT_GRAY
	#define VALUE_FORE_COLOR	C_BLACK
#else
	#define BACK_COLOR		C_BLACK
	#define FORE_COLOR		C_WHITE
	#define TITLE_BACK_COLOR	C_LIGHT_GRAY
	#define TITLE_FORE_COLOR	C_BLACK
	#define HEADER_BACK_COLOR	C_LIGHT_GRAY
	#define HEADER_FORE_COLOR	C_BLACK
	#define BUTTON_BACK_COLOR	C_LIGHT_GRAY
	#define BUTTON_HIGH_COLOR	C_GRAY
	#define VALUE_BACK_COLOR	C_LIGHT_GRAY
	#define VALUE_FORE_COLOR	C_BLACK
#endif

#endif
