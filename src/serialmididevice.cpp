//
// serialmididevice.cpp
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
#include "serialmididevice.h"
#include <circle/sysconfig.h>
#include <assert.h>

CSerialMIDIDevice::CSerialMIDIDevice (CMiniSynthesizer *pSynthesizer, CInterruptSystem *pInterrupt,
				      CSynthConfig *pConfig)
:	CMIDIDevice (pSynthesizer, pConfig),
#if RASPPI <= 3 && defined (USE_USB_FIQ)
	m_Serial (pInterrupt, FALSE),
#else
	m_Serial (pInterrupt, TRUE),
#endif
	m_nSerialState (0)
{
}

CSerialMIDIDevice::~CSerialMIDIDevice (void)
{
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
			if (   (uchData & 0x80) == 0x80		// status byte, all channels
			    && (uchData & 0xF0) != 0xF0)	// ignore system messages
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

			if (   (m_SerialMessage[0] & 0xE0) == 0xC0
			    || m_nSerialState == 3)		// message is complete
			{
				MIDIMessageHandler (m_SerialMessage, m_nSerialState);

				m_nSerialState = 0;
			}
			break;

		default:
			assert (0);
			break;
		}
	}
}
