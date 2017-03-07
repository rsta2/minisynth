//
// envelopegenerator.cpp
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
#include "envelopegenerator.h"
#include "config.h"
#include <assert.h>

CEnvelopeGenerator::CEnvelopeGenerator (void)
:	m_nAttackMsec (200),
	m_nDecayMsec (5000),
	m_fSustainLevel (0.5),
	m_nReleaseMsec (500),
	m_State (EnvelopeStateIdle),
	m_nSampleCount (0),
	m_fOutputLevel (0.0)
{
}

CEnvelopeGenerator::~CEnvelopeGenerator (void)
{
}

void CEnvelopeGenerator::SetAttack (unsigned nMilliSeconds)
{
	m_nAttackMsec = nMilliSeconds;
}

void CEnvelopeGenerator::SetDecay (unsigned nMilliSeconds)
{
	assert (nMilliSeconds > 0);
	m_nDecayMsec = nMilliSeconds;
}

void CEnvelopeGenerator::SetSustain (float fLevel)
{
	assert (0.0 <= fLevel && fLevel <= 1.0);
	m_fSustainLevel = fLevel;

}

void CEnvelopeGenerator::SetRelease (unsigned nMilliSeconds)
{
	m_nReleaseMsec = nMilliSeconds;
}

void CEnvelopeGenerator::NoteOn (float fVelocityLevel)
{
	assert (   m_State == EnvelopeStateIdle
		|| m_State == EnvelopeStateRelease);
	m_State = EnvelopeStateAttack;

	assert (0.0 < fVelocityLevel && fVelocityLevel <= 1.0);
	m_fVelocityLevel = fVelocityLevel;

	m_nSampleCount = 0;
	m_fOutputLevel = 0.0;
}

void CEnvelopeGenerator::NoteOff (void)
{
	if (m_State != EnvelopeStateIdle)
	{
		m_State = EnvelopeStateRelease;

		m_nSampleCount = 0;
		m_fReleaseLevel = m_fOutputLevel;
	}
}

TEnvelopeState CEnvelopeGenerator::GetState (void) const
{
	return m_State;
}

void CEnvelopeGenerator::NextSample (void)
{
	if (++m_nSampleCount == 0)	// may wrap
	{
		m_nSampleCount = (unsigned) -1;
	}

	switch (m_State)
	{
	case EnvelopeStateIdle:
		break;

	case EnvelopeStateAttack:
		if (CalculateLevel (0.0, m_fVelocityLevel, m_nAttackMsec))
		{
			m_nSampleCount = 0;
			m_State = EnvelopeStateDecay;
		}
		break;

	case EnvelopeStateDecay:
		if (CalculateLevel (m_fVelocityLevel, m_fSustainLevel*m_fVelocityLevel, m_nDecayMsec))
		{
			m_nSampleCount = 0;
			m_State = EnvelopeStateSustain;
		}

		if (m_fOutputLevel == 0.0)
		{
			m_State = EnvelopeStateIdle;
		}
		break;

	case EnvelopeStateSustain:
		break;

	case EnvelopeStateRelease:
		if (CalculateLevel (m_fReleaseLevel, 0.0, m_nReleaseMsec))
		{
			m_State = EnvelopeStateIdle;
		}
		break;

	default:
		assert (0);
		break;
	}
}

float CEnvelopeGenerator::GetOutputLevel (void) const
{
	return m_fOutputLevel;
}

boolean CEnvelopeGenerator::CalculateLevel (float fPrevLevel, float fNextLevel, unsigned nMsDelay)
{
	if (nMsDelay == 0)
	{
		m_fOutputLevel = fNextLevel;

		return TRUE;
	}

	float fMsElapsed = m_nSampleCount * 1000.0 / SAMPLE_RATE;

	m_fOutputLevel = fPrevLevel + (fNextLevel-fPrevLevel) * (fMsElapsed / nMsDelay);
	if (m_fOutputLevel < 0.0)
	{
		m_fOutputLevel = 0.0;

		return TRUE;
	}
	if (m_fOutputLevel > 1.0)
	{
		m_fOutputLevel = 1.0;

		return TRUE;
	}

	return fMsElapsed >= nMsDelay;
}
