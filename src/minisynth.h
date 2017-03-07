//
// minisynth.h
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
#ifndef _minisynth_h
#define _minisynth_h

#include <circle/pwmsounddevice2.h>
#include <circle/interrupt.h>
#include <circle/types.h>
#include "patch.h"
#include "pckeyboard.h"
#include "voice.h"
#include "config.h"

class CMiniSynthesizer : public CPWMSoundDevice2
{
public:
	CMiniSynthesizer (CInterruptSystem *pInterrupt);
	~CMiniSynthesizer (void);

	boolean Initialize (void);

	void SetPatch (CPatch *pPatch);

	void NoteOn (u8 ucKeyNumber, u8 ucVelocity = VELOCITY_DEFAULT);	// MIDI key number and velocity
	void NoteOff (u8 ucKeyNumber);

	unsigned GetChunk (u32 *pBuffer, unsigned nChunkSize);

private:
	CPCKeyboard m_Keyboard;

	CVoice *m_pVoice[VOICES];

	unsigned m_nNullLevel;
	unsigned m_nVolumeLevel;
};

#endif
