//
// parameter.cpp
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
#include "parameter.h"
#include <assert.h>

CParameter::CParameter (const char *pName, TParameterType Type,
			unsigned nMinimum, unsigned nMaximum, unsigned nStep, unsigned nDefault,
			const char *pHelp)
:	m_pName (pName),
	m_Type (Type),
	m_nMinimum (nMinimum),
	m_nMaximum (nMaximum),
	m_nStep (nStep),
	m_nDefault (nDefault),
	m_pHelp (pHelp),
	m_nValue (nDefault)
{
	assert (m_pName != 0);
	assert (m_Type < ParameterTypeUnknown);
	assert (m_nMinimum < m_nMaximum);
	assert (m_nMinimum <= m_nDefault && m_nDefault <= m_nMaximum);
	assert (m_pHelp != 0);
}

CParameter::~CParameter (void)
{
}

const char *CParameter::GetName (void)
{
	return m_pName;
}

unsigned CParameter::GetDefault (void) const
{
	return m_nDefault;
}

const char *CParameter::GetHelp (void)
{
	return m_pHelp;
}

unsigned CParameter::Get (void) const
{
	return m_nValue;
}

void CParameter::Set (unsigned nValue)
{
	if (m_nMinimum <= nValue && nValue <= m_nMaximum)
	{
		m_nValue = nValue;
	}
}

boolean CParameter::Down (void)
{
	unsigned nNewValue = m_nValue - m_nStep;
	if (m_nMinimum <= nNewValue && nNewValue <= m_nMaximum)
	{
		m_nValue = nNewValue;

		return TRUE;
	}

	return FALSE;
}

boolean CParameter::Up (void)
{
	unsigned nNewValue = m_nValue + m_nStep;
	if (m_nMinimum <= nNewValue && nNewValue <= m_nMaximum)
	{
		m_nValue = nNewValue;

		return TRUE;
	}

	return FALSE;
}

const char *CParameter::GetString (void)
{
	static const char *Waveforms[] =	// must match TWaveform in oscillator.h
	{
		"Sine",
		"Square",
		"Sawtooth",
		"Triangle",
		"Pulse12",
		"Pulse25"
	};

	switch (m_Type)
	{
	case ParameterWaveform:
		assert (m_nValue < sizeof Waveforms / sizeof Waveforms[0]);
		return Waveforms[m_nValue];

	case ParameterFrequency:
		m_String.Format ("%u Hz", m_nValue);
		return m_String;

	case ParameterTime:
		m_String.Format ("%u ms", m_nValue);
		return m_String;

	case ParameterPercent:
		m_String.Format ("%u %", m_nValue);
		return m_String;

	default:
		assert (0);
		return "";
	}
}
