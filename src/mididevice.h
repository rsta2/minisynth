//
// mididevice.h
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2017-2022  R. Stange <rsta2@o2online.de>
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
#ifndef _mididevice_h
#define _mididevice_h

#include <circle/types.h>
#include "synthconfig.h"

class CMiniSynthesizer;

class CMIDIDevice
{
public:
	CMIDIDevice (CMiniSynthesizer *pSynthesizer, CSynthConfig *pConfig);
	~CMIDIDevice (void);

protected:
	void MIDIMessageHandler (const u8 *pMessage, size_t nLength);

private:
	CMiniSynthesizer *m_pSynthesizer;
	CSynthConfig *m_pConfig;
};

#endif
