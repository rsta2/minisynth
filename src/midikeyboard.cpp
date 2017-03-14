//
// midikeyboard.cpp
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
#include "midikeyboard.h"
#include "minisynth.h"
#include <circle/devicenameservice.h>
#include <circle/usb/usbmidi.h>
#include <assert.h>

#define MIDI_NOTE_OFF	0b1000
#define MIDI_NOTE_ON	0b1001

CMIDIKeyboard *CMIDIKeyboard::s_pThis = 0;

CMIDIKeyboard::CMIDIKeyboard (CMiniSynthesizer *pSynthesizer)
:	m_pSynthesizer (pSynthesizer)
{
	s_pThis = this;
}

CMIDIKeyboard::~CMIDIKeyboard (void)
{
	m_pSynthesizer = 0;

	s_pThis = 0;
}

boolean CMIDIKeyboard::Initialize (void)
{
	CUSBMIDIDevice *pMIDIDevice =
		(CUSBMIDIDevice *) CDeviceNameService::Get ()->GetDevice ("umidi1", FALSE);
	if (pMIDIDevice == 0)
	{
		return FALSE;
	}

	pMIDIDevice->RegisterPacketHandler (MIDIPacketHandler);

	return TRUE;
}

void CMIDIKeyboard::MIDIPacketHandler (unsigned nCable, u8 *pPacket, unsigned nLength)
{
	assert (s_pThis != 0);
	assert (s_pThis->m_pSynthesizer != 0);

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
				s_pThis->m_pSynthesizer->NoteOn (ucKeyNumber, ucVelocity);
			}
		}
		else
		{
			s_pThis->m_pSynthesizer->NoteOff (ucKeyNumber);
		}
	}
	else if (ucType == MIDI_NOTE_OFF)
	{
		s_pThis->m_pSynthesizer->NoteOff (ucKeyNumber);
	}
}
