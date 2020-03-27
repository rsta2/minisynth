//
// midikeyboard.cpp
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
#include "midikeyboard.h"
#include <circle/devicenameservice.h>
#include <circle/usb/usbmidi.h>
#include <assert.h>

CMIDIKeyboard *CMIDIKeyboard::s_pThis = 0;

CMIDIKeyboard::CMIDIKeyboard (CMiniSynthesizer *pSynthesizer)
:	CMIDIDevice (pSynthesizer)
{
	s_pThis = this;
}

CMIDIKeyboard::~CMIDIKeyboard (void)
{
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
	s_pThis->MIDIMessageHandler (pPacket, nLength);
}
