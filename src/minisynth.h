//
// minisynth.h
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2017-2024  R. Stange <rsta2@o2online.de>
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
#include <circle/sound/pwmsoundbasedevice.h>
#include <circle/sound/i2ssoundbasedevice.h>
#include <circle/sound/usbsoundbasedevice.h>
#include <circle/string.h>
#include <circle/types.h>
#include "synthconfig.h"
#include "patch.h"
#include "midikeyboard.h"
#include "pckeyboard.h"
#include "serialmididevice.h"
#include "voicemanager.h"
#include "config.h"

// That all runs on core 0. SetPatch() gets called from the GUI and may be
// interrupted by the other routines. NoteOn/Off() is IRQ-triggered by the USB IRQ
// handler. GetChunk() is IRQ-triggered by the PWM DMA IRQ handler. IRQ handlers
// cannot be interrupted again. Thus the execution of these routines (except
// SetPatch()) is mutual-exclusive. There is no need for explicit synchronization,
// but setting a single parameter in SetPatch() must be atomic.

class CMiniSynthesizer
{
public:
	CMiniSynthesizer (CSynthConfig *pConfig, CInterruptSystem *pInterrupt);
	virtual ~CMiniSynthesizer (void);

	boolean Initialize (void);

	virtual boolean Start (void) = 0;
	virtual boolean IsActive (void) = 0;

	void Process (boolean bPlugAndPlayUpdated);

	void SetPatch (CPatch *pPatch);

	void NoteOn (u8 ucKeyNumber, u8 ucVelocity = VELOCITY_DEFAULT);	// MIDI key number and velocity
	void NoteOff (u8 ucKeyNumber);

	boolean ConfigUpdated (void);
	void ControlChange (u8 ucFunction, u8 ucValue);
	void ProgramChange (u8 ucProgram);

#ifdef SHOW_STATUS
	const char *GetStatus (void);
#endif

protected:
	void GlobalLock (void);
	void GlobalUnlock (void);

private:
	CSynthConfig *m_pConfig;

	CMIDIKeyboard m_MIDIKeyboard0;
	CMIDIKeyboard m_MIDIKeyboard1;
	CPCKeyboard   m_Keyboard;

	CSerialMIDIDevice m_SerialMIDI;
	boolean m_bUseSerial;

	unsigned m_nConfigRevisionWrite;
	unsigned m_nConfigRevisionRead;

protected:
	CVoiceManager m_VoiceManager;

	float m_fVolume;

#ifdef SHOW_STATUS
	CString m_Status;
	unsigned m_nMaxDelayTicks;
#endif
};

//// PWM //////////////////////////////////////////////////////////////////////

#if RASPPI <= 4

class CMiniSynthesizerPWM : public CMiniSynthesizer, public CPWMSoundBaseDevice
{
public:
	CMiniSynthesizerPWM (CSynthConfig *pConfig, CInterruptSystem *pInterrupt);

	boolean Start (void);
	boolean IsActive (void);

private:
	unsigned GetChunk (u32 *pBuffer, unsigned nChunkSize);

private:
	unsigned m_nMaxLevel;
	unsigned m_nNullLevel;
	boolean m_bChannelsSwapped;
};

#endif

//// I2S //////////////////////////////////////////////////////////////////////

class CMiniSynthesizerI2S : public CMiniSynthesizer, public CI2SSoundBaseDevice
{
public:
	CMiniSynthesizerI2S (CSynthConfig *pConfig, CInterruptSystem *pInterrupt,
			     CI2CMaster *pI2CMaster);

	boolean Start (void);
	boolean IsActive (void);

private:
	unsigned GetChunk (u32 *pBuffer, unsigned nChunkSize);

private:
	int m_nMinLevel;
	int m_nMaxLevel;
	boolean m_bChannelsSwapped;
};

//// USB //////////////////////////////////////////////////////////////////////

#if RASPPI >= 4

class CMiniSynthesizerUSB : public CMiniSynthesizer, public CUSBSoundBaseDevice
{
public:
	CMiniSynthesizerUSB (CSynthConfig *pConfig, CInterruptSystem *pInterrupt);

	boolean Start (void);
	boolean IsActive (void);

private:
	unsigned GetChunk (s16 *pBuffer, unsigned nChunkSize);
	unsigned GetChunk (u32 *pBuffer, unsigned nChunkSize);

private:
	int m_nMinLevel;
	int m_nMaxLevel;
	boolean m_bChannelsSwapped;
};

#endif

#endif
