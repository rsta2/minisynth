//
// midiccmap.h
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
#include "midiccmap.h"
#include "config.h"
#include <assert.h>

CMIDICCMap::CMIDICCMap (FATFS *pFileSystem)
:	m_Properties (DRIVE "/midi-cc.txt", pFileSystem)
{
	for (unsigned i = MIDICC_MIN; i <= MIDICC_MAX; i++)
	{
		m_Map[i] = SynthParameterUnknown;
	}
}

CMIDICCMap::~CMIDICCMap (void)
{
}

boolean CMIDICCMap::Load (void)
{
	boolean bResult = m_Properties.Load ();
	if (!bResult)
	{
		m_Properties.RemoveAll ();
	}

	for (unsigned i = 0; i < SynthParameterUnknown; i++)
	{
		unsigned nMIDICC =
			m_Properties.GetNumber (CPatch::GetParameterName ((TSynthParameter) i), 0);

		if (MIDICC_MIN <= nMIDICC && nMIDICC <= MIDICC_MAX)
		{
			m_Map[nMIDICC] = (TSynthParameter) i;
		}
	}

	return bResult;
}

TSynthParameter CMIDICCMap::Map (u8 ucMIDICC) const
{
	if (MIDICC_MIN <= ucMIDICC && ucMIDICC <= MIDICC_MAX)
	{
		return m_Map[ucMIDICC];
	}

	return SynthParameterUnknown;
}
