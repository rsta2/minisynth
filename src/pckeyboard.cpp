//
// pckeyboard.cpp
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
#include "pckeyboard.h"
#include "minisynth.h"
#include <circle/devicenameservice.h>
#include <circle/usb/usbkeyboard.h>
#include <circle/util.h>
#include <assert.h>

struct TKeyInfo
{
	char	KeyCode;	// upper case letter or digit
	u8	KeyNumber;	// MIDI number
};

// KeyCode is valid for standard QWERTY keyboard
static TKeyInfo KeyTable[] =
{
	{',', 52}, // C5

	{'M', 51}, // B4
	{'J', 50}, // A#4
	{'N', 49}, // A4
	{'H', 48}, // G#4
	{'B', 47}, // G4
	{'G', 46}, // F#4
	{'V', 45}, // F4
	{'C', 44}, // E4
	{'D', 43}, // D#4
	{'X', 42}, // D4
	{'S', 41}, // C#4
	{'Z', 40}, // C4

	{'U', 39}, // B3
	{'7', 38}, // A#3
	{'Y', 37}, // A3
	{'6', 36}, // G#3
	{'T', 35}, // G3
	{'5', 34}, // F#3
	{'R', 33}, // F3
	{'E', 32}, // E3
	{'3', 31}, // D#3
	{'W', 30}, // D3
	{'2', 29}, // C#3
	{'Q', 28}  // C3
};

CPCKeyboard *CPCKeyboard::s_pThis = 0;

CPCKeyboard::CPCKeyboard (CMiniSynthesizer *pSynthesizer)
:	m_pSynthesizer (pSynthesizer)
{
	s_pThis = this;

	memset (m_LastKeys, 0, sizeof m_LastKeys);
}

CPCKeyboard::~CPCKeyboard (void)
{
	m_pSynthesizer = 0;

	s_pThis = 0;
}

boolean CPCKeyboard::Initialize (void)
{
	CUSBKeyboardDevice *pKeyboard =
		(CUSBKeyboardDevice *) CDeviceNameService::Get ()->GetDevice ("ukbd1", FALSE);
	if (pKeyboard == 0)
	{
		return FALSE;
	}

	pKeyboard->RegisterKeyStatusHandlerRaw (KeyStatusHandlerRaw);

	return TRUE;
}

void CPCKeyboard::KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6])
{
	assert (s_pThis != 0);
	assert (s_pThis->m_pSynthesizer != 0);

	// report released keys
	for (unsigned i = 0; i < 6; i++)
	{
		u8 ucKeyCode = s_pThis->m_LastKeys[i];
		if (   ucKeyCode != 0
		    && !FindByte (RawKeys, ucKeyCode, 6))
		{
			u8 ucKeyNumber = GetKeyNumber (ucKeyCode);
			if (ucKeyNumber != 0)
			{
				s_pThis->m_pSynthesizer->NoteOff (ucKeyNumber);
			}
		}
	}

	// report pressed keys
	for (unsigned i = 0; i < 6; i++)
	{
		u8 ucKeyCode = RawKeys[i];
		if (   ucKeyCode != 0
		    && !FindByte (s_pThis->m_LastKeys, ucKeyCode, 6))
		{
			u8 ucKeyNumber = GetKeyNumber (ucKeyCode);
			if (ucKeyNumber != 0)
			{
				s_pThis->m_pSynthesizer->NoteOn (ucKeyNumber);
			}
		}
	}

	memcpy (s_pThis->m_LastKeys, RawKeys, sizeof RawKeys);
}

u8 CPCKeyboard::GetKeyNumber (u8 ucKeyCode)
{
	char chKey;
	if (0x04 <= ucKeyCode && ucKeyCode <= 0x1D)
	{
		chKey = ucKeyCode-'\x04'+'A';	// key code of 'A' is 0x04
	}
	else if (0x1E <= ucKeyCode && ucKeyCode <= 0x26)
	{
		chKey = ucKeyCode-'\x1E'+'1';	// key code of '1' is 0x1E
	}
	else if (ucKeyCode <= 0x36)
	{
		chKey = ',';			// key code of ',' is 0x36
	}
	else
	{
		return 0;
	}

	for (unsigned i = 0; i < sizeof KeyTable / sizeof KeyTable[0]; i++)
	{
		if (KeyTable[i].KeyCode == chKey)
		{
			return KeyTable[i].KeyNumber;
		}
	}

	return 0;
}

boolean CPCKeyboard::FindByte (const u8 *pBuffer, u8 ucByte, unsigned nLength)
{
	while (nLength-- > 0)
	{
		if (*pBuffer++ == ucByte)
		{
			return TRUE;
		}
	}

	return FALSE;
}
