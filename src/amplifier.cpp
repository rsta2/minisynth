//
// amplifier.cpp
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
#include "amplifier.h"
#include <assert.h>

CAmplifier::CAmplifier (CSynthModule *pInput, CSynthModule *pModulator, CSynthModule *pEnvelope)
:	m_pInput (pInput),
	m_pModulator (pModulator),
	m_pEnvelope (pEnvelope),
	m_fModulationVolume (0.0),
	m_fOutputLevel (0.0)
{
}

CAmplifier::~CAmplifier (void)
{
	m_pInput = 0;
	m_pModulator = 0;
	m_pEnvelope = 0;
}

void CAmplifier::SetModulationVolume (float fVolume)
{
	assert (0.0 <= fVolume && fVolume <= 1.0);
	m_fModulationVolume = fVolume;
}

void CAmplifier::NextSample (void)
{
	assert (m_pInput != 0);
	assert (m_pModulator != 0);
	assert (m_pEnvelope != 0);

	m_fOutputLevel  = m_pInput->GetOutputLevel ();
	m_fOutputLevel *= 1.0 + m_pModulator->GetOutputLevel ()*m_fModulationVolume;
	m_fOutputLevel *= m_pEnvelope->GetOutputLevel ();
}

float CAmplifier::GetOutputLevel (void) const
{
	return m_fOutputLevel;
}
