//
// parameter.cpp
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
#include "parameter.h"
#include <circle/util.h>
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
	nNewValue = (nNewValue + m_nStep-1) / m_nStep * m_nStep;
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
	nNewValue = nNewValue / m_nStep * m_nStep;
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
		"Pulse25",
		"Noise"
	};

	switch (m_Type)
	{
	case ParameterWaveform:
		assert (m_nValue < sizeof Waveforms / sizeof Waveforms[0]);
		return Waveforms[m_nValue];

	case ParameterFrequency:
		m_String.Format ("%u Hz", m_nValue);
		return m_String;

	case ParameterFrequencyTenth:
		m_String.Format ("%.1f Hz", m_nValue / 10.0);
		return m_String;

	case ParameterTime:
		m_String.Format ("%u ms", m_nValue);
		return m_String;

	case ParameterPercent:
		m_String.Format ("%u %", m_nValue);
		return m_String;

	case ParameterChannel:
		if (m_nValue == 0)
		{
			return "Omni Mode";
		}
		m_String.Format ("%u", m_nValue);
		return m_String;

	default:
		assert (0);
		return "";
	}
}

boolean CParameter::IsEditable (void) const
{
	return    m_Type != ParameterWaveform
	       && m_Type != ParameterChannel;
}

const char *CParameter::GetEditString (void)
{
	assert (   m_Type != ParameterWaveform
		&& m_Type != ParameterChannel);
	if (m_Type != ParameterFrequencyTenth)
	{
		m_String.Format ("%u", m_nValue);
	}
	else
	{
		m_String.Format ("%.1f", m_nValue / 10.0);
	}

	return m_String;
}

void CParameter::SetEditString (const char *pString)
{
	assert (pString != 0);

	char Buffer[20];
	strncpy (Buffer, pString, sizeof Buffer);
	Buffer[sizeof Buffer-1] = '\0';

	char *pSavePtr;
	char *p = strtok_r (Buffer, ".", &pSavePtr);
	if (!p)
	{
		return;
	}

	char *pEnd = 0;
	unsigned long ulValue = strtoul (p, &pEnd, 10);
	if (   pEnd != 0
	    && *pEnd != '\0')
	{
		return;
	}

	if (m_Type == ParameterFrequencyTenth)
	{
		ulValue *= 10;

		p = strtok_r (0, "", &pSavePtr);
		if (p)
		{
			unsigned long ulTenth = strtoul (p, &pEnd, 10);
			if (   (   pEnd != 0
				&& *pEnd != '\0')
			    || ulTenth > 9)
			{
				return;
			}

			ulValue += ulTenth;
		}
	}

	Set ((unsigned) ulValue);
}
