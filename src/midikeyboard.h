//
// midikeyboard.h
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2017  R. Stange <rsta2@o2online.de>
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
#ifndef _midikeyboard_h
#define _midikeyboard_h

#include <circle/types.h>

class CMiniSynthesizer;

class CMIDIKeyboard
{
public:
	CMIDIKeyboard (CMiniSynthesizer *pSynthesizer);
	~CMIDIKeyboard (void);

	boolean Initialize (void);

private:
	static void MIDIPacketHandler (unsigned nCable, u8 *pPacket, unsigned nLength);

private:
	CMiniSynthesizer *m_pSynthesizer;

	static CMIDIKeyboard *s_pThis;
};

#endif
