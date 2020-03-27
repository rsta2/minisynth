//
// mididevice.cpp
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
#include "mididevice.h"
#include "minisynth.h"
#include <assert.h>

#define MIDI_NOTE_OFF	0b1000
#define MIDI_NOTE_ON	0b1001

CMIDIDevice::CMIDIDevice (CMiniSynthesizer *pSynthesizer)
:	m_pSynthesizer (pSynthesizer)
{
}

CMIDIDevice::~CMIDIDevice (void)
{
	m_pSynthesizer = 0;
}

void CMIDIDevice::MIDIMessageHandler (const u8 *pMessage, size_t nLength)
{
	assert (m_pSynthesizer != 0);

	// The packet contents are just normal MIDI data - see
	// https://www.midi.org/specifications/item/table-1-summary-of-midi-message

	if (nLength < 3)
	{
		return;
	}

	u8 ucStatus    = pMessage[0];
	//u8 ucChannel   = ucStatus & 0x0F;
	u8 ucType      = ucStatus >> 4;
	u8 ucKeyNumber = pMessage[1];
	u8 ucVelocity  = pMessage[2];

	if (ucType == MIDI_NOTE_ON)
	{
		if (ucVelocity > 0)
		{
			if (ucVelocity <= 127)
			{
				m_pSynthesizer->NoteOn (ucKeyNumber, ucVelocity);
			}
		}
		else
		{
			m_pSynthesizer->NoteOff (ucKeyNumber);
		}
	}
	else if (ucType == MIDI_NOTE_OFF)
	{
		m_pSynthesizer->NoteOff (ucKeyNumber);
	}
}
