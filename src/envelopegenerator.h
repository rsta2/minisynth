//
// envelopegenerator.h
//
// ADSR envelope generator
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
#ifndef _envelopegenerator_h
#define _envelopegenerator_h

#include "synthmodule.h"
#include <circle/types.h>

enum TEnvelopeState
{
	EnvelopeStateIdle,
	EnvelopeStateAttack,
	EnvelopeStateDecay,
	EnvelopeStateSustain,
	EnvelopeStateRelease,
	EnvelopeStateUnknown
};

class CEnvelopeGenerator : public CSynthModule
{
public:
	CEnvelopeGenerator (void);
	~CEnvelopeGenerator (void);

	void SetAttack (unsigned nMilliSeconds);
	void SetDecay (unsigned nMilliSeconds);
	void SetSustain (float fLevel);			// [0.0, 1.0]
	void SetRelease (unsigned nMilliSeconds);

	void NoteOn (float fVelocityLevel = 1.0);	// (0.0, 1.0]
	void NoteOff (void);

	TEnvelopeState GetState (void) const;

	void NextSample (void);
	float GetOutputLevel (void) const;		// returns [0.0, 1.0]

private:
	// returns TRUE if next phase starts
	boolean CalculateLevel (float fPrevLevel, float fNextLevel, unsigned nMsDelay);

private:
	unsigned m_nAttackMsec;
	unsigned m_nDecayMsec;
	float    m_fSustainLevel;
	unsigned m_nReleaseMsec;

	TEnvelopeState m_State;

	float m_fVelocityLevel;				// [0.0, 1.0]
	unsigned m_nSampleCount;
	float m_fReleaseLevel;

	float m_fOutputLevel;
};

#endif
