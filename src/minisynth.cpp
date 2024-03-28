//
// minisynth.cpp
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
#include "minisynth.h"
#include "math.h"
#include "config.h"
#include <circle/timer.h>
#include <circle/synchronize.h>
#include <circle/memory.h>
#include <circle/logger.h>
#include <assert.h>

static const char FromMiniSynth[] = "synth";

CMiniSynthesizer::CMiniSynthesizer (CSynthConfig *pConfig, CInterruptSystem *pInterrupt)
:	m_pConfig (pConfig),
	m_MIDIKeyboard0 (this, pConfig, 0),
	m_MIDIKeyboard1 (this, pConfig, 1),
	m_Keyboard (this),
	m_SerialMIDI (this, pInterrupt, pConfig),
	m_bUseSerial (FALSE),
	m_nConfigRevisionWrite (0),
	m_nConfigRevisionRead (0),
	m_VoiceManager (CMemorySystem::Get ()),
	m_fVolume (0.0)
#ifdef SHOW_STATUS
	, m_nMaxDelayTicks (0)
#endif
{
}

CMiniSynthesizer::~CMiniSynthesizer (void)
{
}

boolean CMiniSynthesizer::Initialize (void)
{
	if (m_SerialMIDI.Initialize ())
	{
		CLogger::Get ()->Write (FromMiniSynth, LogNotice, "Serial MIDI interface enabled");

		m_bUseSerial = TRUE;

		return m_VoiceManager.Initialize ();
	}

	return FALSE;
}

void CMiniSynthesizer::Process (boolean bPlugAndPlayUpdated)
{
	m_MIDIKeyboard0.Process (bPlugAndPlayUpdated);
	m_MIDIKeyboard1.Process (bPlugAndPlayUpdated);

	m_Keyboard.Process (bPlugAndPlayUpdated);

	if (m_bUseSerial)
	{
		m_SerialMIDI.Process ();
	}
}

void CMiniSynthesizer::SetPatch (CPatch *pPatch)
{
	assert (pPatch != 0);

	GlobalLock ();

	m_VoiceManager.SetPatch (pPatch);

	m_fVolume = powf (pPatch->GetParameter (SynthVolume) / 100.0, 3.3f); // apply some curve

	GlobalUnlock ();
}

void CMiniSynthesizer::NoteOn (u8 ucKeyNumber, u8 ucVelocity)
{
	// apply velocity curve
	assert (m_pConfig != 0);
	ucVelocity = m_pConfig->MapVelocity (ucVelocity);

	GlobalLock ();

	m_VoiceManager.NoteOn (ucKeyNumber, ucVelocity);

	GlobalUnlock ();
}

void CMiniSynthesizer::NoteOff (u8 ucKeyNumber)
{
	GlobalLock ();

	m_VoiceManager.NoteOff (ucKeyNumber);

	GlobalUnlock ();
}

boolean CMiniSynthesizer::ConfigUpdated (void)
{
	unsigned nConfigRevisionWrite = m_nConfigRevisionWrite;
	if (nConfigRevisionWrite != m_nConfigRevisionRead)
	{
		m_nConfigRevisionRead = nConfigRevisionWrite;

		return TRUE;
	}

	return FALSE;
}

void CMiniSynthesizer::ControlChange (u8 ucFunction, u8 ucValue)
{
	assert (m_pConfig != 0);
	TSynthParameter Parameter = m_pConfig->MapMIDICC (ucFunction);
	if (Parameter >= SynthParameterUnknown)
	{
		return;
	}

	GlobalLock ();

	CPatch *pPatch = m_pConfig->GetActivePatch ();
	assert (pPatch != 0);

	pPatch->SetMIDIParameter (Parameter, ucValue);
	SetPatch (pPatch);

	m_nConfigRevisionWrite++;

	GlobalUnlock ();
}

void CMiniSynthesizer::ProgramChange (u8 ucProgram)
{
	assert (m_pConfig != 0);

	GlobalLock ();

	if (ucProgram < PATCHES)
	{
		m_pConfig->SetActivePatchNumber (ucProgram);
		SetPatch (m_pConfig->GetActivePatch ());
		m_nConfigRevisionWrite++;
	}

	GlobalUnlock ();
}

#ifdef SHOW_STATUS

const char *CMiniSynthesizer::GetStatus (void)
{
	m_Status.Format ("%u ms", m_nMaxDelayTicks * 1000 / CLOCKHZ);

	return m_Status;
}

#endif

void CMiniSynthesizer::GlobalLock (void)
{
	EnterCritical (IRQ_LEVEL);
}

void CMiniSynthesizer::GlobalUnlock (void)
{
	LeaveCritical ();
}

//// PWM //////////////////////////////////////////////////////////////////////

CMiniSynthesizerPWM::CMiniSynthesizerPWM (CSynthConfig *pConfig,
					  CInterruptSystem *pInterrupt)
:	CMiniSynthesizer (pConfig, pInterrupt),
	CPWMSoundBaseDevice (pInterrupt, SAMPLE_RATE),
	m_nMaxLevel (GetRangeMax ()-1),
	m_nNullLevel (m_nMaxLevel / 2),
	m_bChannelsSwapped (AreChannelsSwapped ())
{
}

boolean CMiniSynthesizerPWM::Start (void)
{
	return CPWMSoundBaseDevice::Start ();
}

boolean CMiniSynthesizerPWM::IsActive (void)
{
	return CPWMSoundBaseDevice::IsActive ();
}

unsigned CMiniSynthesizerPWM::GetChunk (u32 *pBuffer, unsigned nChunkSize)
{
#ifdef SHOW_STATUS
	unsigned nTicks = CTimer::GetClockTicks ();
#endif

	GlobalLock ();

	unsigned nResult = nChunkSize;

	float fVolumeLevel = m_fVolume * m_nMaxLevel/2;

	for (; nChunkSize > 0; nChunkSize -= 2)		// fill the whole buffer
	{
		m_VoiceManager.NextSample ();

		float fLevelLeft = m_VoiceManager.GetOutputLevelLeft ();
		int nLevelLeft = (int) (fLevelLeft*fVolumeLevel + m_nNullLevel);
		if (nLevelLeft > (int) m_nMaxLevel)
		{
			nLevelLeft = m_nMaxLevel;
		}
		else if (nLevelLeft < 0)
		{
			nLevelLeft = 0;
		}

		float fLevelRight = m_VoiceManager.GetOutputLevelRight ();
		int nLevelRight = (int) (fLevelRight*fVolumeLevel + m_nNullLevel);
		if (nLevelRight > (int) m_nMaxLevel)
		{
			nLevelRight = m_nMaxLevel;
		}
		else if (nLevelRight < 0)
		{
			nLevelRight = 0;
		}

		// for 2 stereo channels
		if (!m_bChannelsSwapped)
		{
			*pBuffer++ = (u32) nLevelLeft;
			*pBuffer++ = (u32) nLevelRight;
		}
		else
		{
			*pBuffer++ = (u32) nLevelRight;
			*pBuffer++ = (u32) nLevelLeft;
		}
	}

#ifdef SHOW_STATUS
	nTicks = CTimer::GetClockTicks () - nTicks;
	if (nTicks > m_nMaxDelayTicks)
	{
		m_nMaxDelayTicks = nTicks;
	}
#endif

	GlobalUnlock ();

	return nResult;
}

//// I2S //////////////////////////////////////////////////////////////////////

CMiniSynthesizerI2S::CMiniSynthesizerI2S (CSynthConfig *pConfig,
					  CInterruptSystem *pInterrupt,
					  CI2CMaster *pI2CMaster)
:	CMiniSynthesizer (pConfig, pInterrupt),
	CI2SSoundBaseDevice (pInterrupt, SAMPLE_RATE, 2048, FALSE, pI2CMaster, DAC_I2C_ADDRESS),
	m_nMinLevel (GetRangeMin ()+1),
	m_nMaxLevel (GetRangeMax ()-1),
	m_bChannelsSwapped (AreChannelsSwapped ())
{
}

boolean CMiniSynthesizerI2S::Start (void)
{
	return CI2SSoundBaseDevice::Start ();
}

boolean CMiniSynthesizerI2S::IsActive (void)
{
	return CI2SSoundBaseDevice::IsActive ();
}

unsigned CMiniSynthesizerI2S::GetChunk (u32 *pBuffer, unsigned nChunkSize)
{
#ifdef SHOW_STATUS
	unsigned nTicks = CTimer::GetClockTicks ();
#endif

	GlobalLock ();

	unsigned nResult = nChunkSize;

	float fVolumeLevel = m_fVolume * m_nMaxLevel;

	for (; nChunkSize > 0; nChunkSize -= 2)		// fill the whole buffer
	{
		m_VoiceManager.NextSample ();

		float fLevelLeft = m_VoiceManager.GetOutputLevelLeft ();
		int nLevelLeft = (int) (fLevelLeft*fVolumeLevel);
		if (nLevelLeft > (int) m_nMaxLevel)
		{
			nLevelLeft = m_nMaxLevel;
		}
		else if (nLevelLeft < m_nMinLevel)
		{
			nLevelLeft = m_nMinLevel;
		}

		float fLevelRight = m_VoiceManager.GetOutputLevelRight ();
		int nLevelRight = (int) (fLevelRight*fVolumeLevel);
		if (nLevelRight > (int) m_nMaxLevel)
		{
			nLevelRight = m_nMaxLevel;
		}
		else if (nLevelRight < m_nMinLevel)
		{
			nLevelRight = m_nMinLevel;
		}

		// for 2 stereo channels
		if (!m_bChannelsSwapped)
		{
			*pBuffer++ = (u32) nLevelLeft;
			*pBuffer++ = (u32) nLevelRight;
		}
		else
		{
			*pBuffer++ = (u32) nLevelRight;
			*pBuffer++ = (u32) nLevelLeft;
		}
	}

#ifdef SHOW_STATUS
	nTicks = CTimer::GetClockTicks () - nTicks;
	if (nTicks > m_nMaxDelayTicks)
	{
		m_nMaxDelayTicks = nTicks;
	}
#endif

	GlobalUnlock ();

	return nResult;
}

//// USB //////////////////////////////////////////////////////////////////////

#if RASPPI >= 4

CMiniSynthesizerUSB::CMiniSynthesizerUSB (CSynthConfig *pConfig,
					  CInterruptSystem *pInterrupt)
:	CMiniSynthesizer (pConfig, pInterrupt),
	CUSBSoundBaseDevice (SAMPLE_RATE),
	m_nMinLevel (GetRangeMin ()+1),
	m_nMaxLevel (GetRangeMax ()-1),
	m_bChannelsSwapped (AreChannelsSwapped ())
{
}

boolean CMiniSynthesizerUSB::Start (void)
{
	return CUSBSoundBaseDevice::Start ();
}

boolean CMiniSynthesizerUSB::IsActive (void)
{
	return CUSBSoundBaseDevice::IsActive ();
}

unsigned CMiniSynthesizerUSB::GetChunk (s16 *pBuffer, unsigned nChunkSize)
{
#ifdef SHOW_STATUS
	unsigned nTicks = CTimer::GetClockTicks ();
#endif

	GlobalLock ();

	unsigned nChannels = GetHWTXChannels ();
	unsigned nResult = nChunkSize;

	float fVolumeLevel = m_fVolume * m_nMaxLevel;

	for (; nChunkSize > 0; nChunkSize -= nChannels)		// fill the whole buffer
	{
		m_VoiceManager.NextSample ();

		float fLevelLeft = m_VoiceManager.GetOutputLevelLeft ();
		int nLevelLeft = (int) (fLevelLeft*fVolumeLevel);
		if (nLevelLeft > (int) m_nMaxLevel)
		{
			nLevelLeft = m_nMaxLevel;
		}
		else if (nLevelLeft < m_nMinLevel)
		{
			nLevelLeft = m_nMinLevel;
		}

		float fLevelRight = m_VoiceManager.GetOutputLevelRight ();
		int nLevelRight = (int) (fLevelRight*fVolumeLevel);
		if (nLevelRight > (int) m_nMaxLevel)
		{
			nLevelRight = m_nMaxLevel;
		}
		else if (nLevelRight < m_nMinLevel)
		{
			nLevelRight = m_nMinLevel;
		}

		assert (nChannels >= 2);
		if (!m_bChannelsSwapped)
		{
			*pBuffer++ = (s16) nLevelLeft;
			*pBuffer++ = (s16) nLevelRight;
		}
		else
		{
			*pBuffer++ = (s16) nLevelRight;
			*pBuffer++ = (s16) nLevelLeft;
		}

		for (unsigned i = 2; i < nChannels; i++)
		{
			*pBuffer++ = 0;
		}
	}

#ifdef SHOW_STATUS
	nTicks = CTimer::GetClockTicks () - nTicks;
	if (nTicks > m_nMaxDelayTicks)
	{
		m_nMaxDelayTicks = nTicks;
	}
#endif

	GlobalUnlock ();

	return nResult;
}

unsigned CMiniSynthesizerUSB::GetChunk (u32 *pBuffer, unsigned nChunkSize)
{
#ifdef SHOW_STATUS
	unsigned nTicks = CTimer::GetClockTicks ();
#endif

	GlobalLock ();

	unsigned nChannels = GetHWTXChannels ();
	unsigned nResult = nChunkSize;

	float fVolumeLevel = m_fVolume * m_nMaxLevel;

	for (; nChunkSize > 0; nChunkSize -= nChannels)		// fill the whole buffer
	{
		m_VoiceManager.NextSample ();

		float fLevelLeft = m_VoiceManager.GetOutputLevelLeft ();
		int nLevelLeft = (int) (fLevelLeft*fVolumeLevel);
		if (nLevelLeft > (int) m_nMaxLevel)
		{
			nLevelLeft = m_nMaxLevel;
		}
		else if (nLevelLeft < m_nMinLevel)
		{
			nLevelLeft = m_nMinLevel;
		}

		float fLevelRight = m_VoiceManager.GetOutputLevelRight ();
		int nLevelRight = (int) (fLevelRight*fVolumeLevel);
		if (nLevelRight > (int) m_nMaxLevel)
		{
			nLevelRight = m_nMaxLevel;
		}
		else if (nLevelRight < m_nMinLevel)
		{
			nLevelRight = m_nMinLevel;
		}

		assert (nChannels >= 2);
		if (!m_bChannelsSwapped)
		{
			*pBuffer = (u32) nLevelLeft;
			pBuffer = (u32 *) ((u8 *) pBuffer + 3);
			*pBuffer = (u32) nLevelRight;
			pBuffer = (u32 *) ((u8 *) pBuffer + 3);
		}
		else
		{
			*pBuffer = (u32) nLevelRight;
			pBuffer = (u32 *) ((u8 *) pBuffer + 3);
			*pBuffer = (u32) nLevelLeft;
			pBuffer = (u32 *) ((u8 *) pBuffer + 3);
		}

		for (unsigned i = 2; i < nChannels; i++)
		{
			*pBuffer = 0;
			pBuffer = (u32 *) ((u8 *) pBuffer + 3);
		}
	}

#ifdef SHOW_STATUS
	nTicks = CTimer::GetClockTicks () - nTicks;
	if (nTicks > m_nMaxDelayTicks)
	{
		m_nMaxDelayTicks = nTicks;
	}
#endif

	GlobalUnlock ();

	return nResult;
}

#endif
