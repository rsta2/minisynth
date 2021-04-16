//
// minisynth.h
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2017-2021  R. Stange <rsta2@o2online.de>
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
#ifndef _minisynth_h
#define _minisynth_h

#include <circle/interrupt.h>
#include <circle/i2cmaster.h>
#include <circle/memory.h>
#include <circle/string.h>
#include <circle/types.h>
#include "synthconfig.h"
#include "patch.h"
#include "midikeyboard.h"
#include "pckeyboard.h"
#include "serialmididevice.h"
#include "voicemanager.h"
#include "config.h"

#ifdef USE_I2S
	#include <circle/i2ssoundbasedevice.h>
#else
	#include <circle/pwmsoundbasedevice.h>
#endif

// That all runs on core 0. SetPatch() gets called from the GUI and may be
// interrupted by the other routines. NoteOn/Off() is IRQ-triggered by the USB IRQ
// handler. GetChunk() is IRQ-triggered by the PWM DMA IRQ handler. IRQ handlers
// cannot be interrupted again. Thus the execution of these routines (except
// SetPatch()) is mutual-exclusive. There is no need for explicit synchronization,
// but setting a single parameter in SetPatch() must be atomic.

class CMiniSynthesizer
#ifdef USE_I2S
	: public CI2SSoundBaseDevice
#else
	: public CPWMSoundBaseDevice
#endif
{
public:
	CMiniSynthesizer (CSynthConfig *pConfig,
			  CInterruptSystem *pInterrupt, CMemorySystem *pMemorySystem,
			  CI2CMaster *pI2CMaster);
	~CMiniSynthesizer (void);

	boolean Initialize (void);

	void Process (boolean bPlugAndPlayUpdated);

	void SetPatch (CPatch *pPatch);

	void NoteOn (u8 ucKeyNumber, u8 ucVelocity = VELOCITY_DEFAULT);	// MIDI key number and velocity
	void NoteOff (u8 ucKeyNumber);

	boolean ConfigUpdated (void);
	void ControlChange (u8 ucFunction, u8 ucValue);
	void ProgramChange (u8 ucProgram);

	unsigned GetChunk (u32 *pBuffer, unsigned nChunkSize);

#ifdef SHOW_STATUS
	const char *GetStatus (void);
#endif

private:
	CSynthConfig *m_pConfig;

	CMIDIKeyboard m_MIDIKeyboard;
	CPCKeyboard   m_Keyboard;

	CSerialMIDIDevice m_SerialMIDI;
	boolean m_bUseSerial;

	CVoiceManager m_VoiceManager;

	unsigned m_nConfigRevisionWrite;
	unsigned m_nConfigRevisionRead;

#ifdef USE_I2S
	int m_nMinLevel;
	int m_nMaxLevel;
	int m_nNullLevel;
	int m_nVolumeLevel;
#else
	unsigned m_nMaxLevel;
	unsigned m_nNullLevel;
	unsigned m_nVolumeLevel;
#endif
	boolean m_bChannelsSwapped;

#ifdef SHOW_STATUS
	CString m_Status;
	unsigned m_nMaxDelayTicks;
#endif
};

#endif
