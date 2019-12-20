//
// serialmididevice.cpp
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2017-2019  R. Stange <rsta2@o2online.de>
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
#include "serialmididevice.h"
#include "minisynth.h"
#include <assert.h>

#define MIDI_NOTE_OFF	0b1000
#define MIDI_NOTE_ON	0b1001

CSerialMIDIDevice::CSerialMIDIDevice (CMiniSynthesizer *pSynthesizer, CInterruptSystem *pInterrupt)
:	m_pSynthesizer (pSynthesizer),
	m_Serial (pInterrupt, TRUE),
	m_nSerialState (0)
{
}

CSerialMIDIDevice::~CSerialMIDIDevice (void)
{
	m_pSynthesizer = 0;
	m_nSerialState = 255;
}

boolean CSerialMIDIDevice::Initialize (void)
{
	return m_Serial.Initialize (31250);
}

void CSerialMIDIDevice::Process (void)
{
	// Read serial MIDI data
	u8 Buffer[100];
	int nResult = m_Serial.Read (Buffer, sizeof Buffer);
	if (nResult <= 0)
	{
		return;
	}

	// Process MIDI messages
	// See: https://www.midi.org/specifications/item/table-1-summary-of-midi-message
	for (int i = 0; i < nResult; i++)
	{
		u8 uchData = Buffer[i];

		switch (m_nSerialState)
		{
		case 0:
		MIDIRestart:
			if ((uchData & 0xE0) == 0x80)		// Note on or off, all channels
			{
				m_SerialMessage[m_nSerialState++] = uchData;
			}
			break;

		case 1:
		case 2:
			if (uchData & 0x80)			// got status when parameter expected
			{
				m_nSerialState = 0;

				goto MIDIRestart;
			}

			m_SerialMessage[m_nSerialState++] = uchData;

			if (m_nSerialState == 3)		// message is complete
			{
				MIDIPacketHandler (0, m_SerialMessage, sizeof m_SerialMessage);

				m_nSerialState = 0;
			}
			break;

		default:
			assert (0);
			break;
		}
	}
}

void CSerialMIDIDevice::MIDIPacketHandler (unsigned nCable, u8 *pPacket, unsigned nLength)
{
	assert (m_pSynthesizer != 0);

	// The packet contents are just normal MIDI data - see
	// https://www.midi.org/specifications/item/table-1-summary-of-midi-message

	if (nLength < 3)
	{
		return;
	}

	u8 ucStatus    = pPacket[0];
	//u8 ucChannel   = ucStatus & 0x0F;
	u8 ucType      = ucStatus >> 4;
	u8 ucKeyNumber = pPacket[1];
	u8 ucVelocity  = pPacket[2];

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
