//
// midiccmap.h
//
// Maps MIDI CC numbers to TSynthParameter
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2021  R. Stange <rsta2@o2online.de>
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
#ifndef _midiccmap_h
#define _midiccmap_h

#include <fatfs/ff.h>
#include <circle/types.h>
#include <Properties/propertiesfatfsfile.h>
#include "patch.h"

#define MIDICC_MIN	1
#define MIDICC_MAX	127

class CMIDICCMap
{
public:
	CMIDICCMap (FATFS *pFileSystem);
	~CMIDICCMap (void);

	boolean Load (void);

	TSynthParameter Map (u8 ucMIDICC) const;

private:
	CPropertiesFatFsFile m_Properties;

	TSynthParameter m_Map[MIDICC_MAX+1];
};

#endif
