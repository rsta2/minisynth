//
// voice.h
//
// One voice in a polyphonic choir
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
#ifndef _voice_h
#define _voice_h

#include "oscillator.h"
#include "envelopegenerator.h"
#include "filter.h"
#include "amplifier.h"
#include "patch.h"
#include <circle/types.h>

enum TVoiceState
{
	VoiceStateIdle,
	VoiceStateActive,
	VoiceStateRelease,
	VoiceStateUnknown
};

class CVoice
{
public:
	CVoice (void);
	~CVoice (void);

	void SetPatch (CPatch *pPatch);

	void NoteOn (u8 ucKeyNumber, u8 ucVelocity);	// MIDI key number and velocity
	void NoteOff (void);

	TVoiceState GetState (void) const;
	u8 GetKeyNumber (void) const;			// returns KEY_NUMBER_NONE if voice is unused
#define KEY_NUMBER_NONE		255

	void NextSample (void);
	float GetOutputLevel (void) const;

private:
	// VCO
	COscillator m_LFO_VCO;
	COscillator m_VCO;

	// VCF
	COscillator m_LFO_VCF;
	CEnvelopeGenerator m_EG_VCF;
	CFilter m_VCF;

	// VCA
	COscillator m_LFO_VCA;
	CEnvelopeGenerator m_EG_VCA;
	CAmplifier m_VCA;

	u8 m_ucKeyNumber;
};

#endif
