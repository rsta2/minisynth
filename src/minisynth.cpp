//
// minisynth.cpp
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
#include "minisynth.h"
#include "math.h"
#include "config.h"
#include <circle/timer.h>
#include <circle/logger.h>
#include <assert.h>

static const char FromMiniSynth[] = "synth";

CMiniSynthesizer::CMiniSynthesizer (CSynthConfig *pConfig,
				    CInterruptSystem *pInterrupt, CMemorySystem *pMemorySystem)
#ifdef USE_I2S
:	CI2SSoundBaseDevice (pInterrupt, SAMPLE_RATE, 2048),
#elif defined (USE_HDMI)
:	CHDMISoundBaseDevice (pInterrupt, SAMPLE_RATE),
#else
:	CPWMSoundBaseDevice (pInterrupt, SAMPLE_RATE),
#endif
	m_pConfig (pConfig),
	m_MIDIKeyboard (this),
	m_Keyboard (this),
	m_SerialMIDI (this, pInterrupt),
	m_bUseSerial (FALSE),
	m_VoiceManager (pMemorySystem),
	m_nConfigRevisionWrite (0),
	m_nConfigRevisionRead (0),
#if defined (USE_I2S) || defined (USE_HDMI)
	m_nMinLevel (GetRangeMin ()+1),
	m_nMaxLevel (GetRangeMax ()-1),
	m_nNullLevel (0),
#else
	m_nMaxLevel (GetRange ()-1),
	m_nNullLevel (m_nMaxLevel / 2),
#endif
	m_nVolumeLevel (0),
	m_bChannelsSwapped (AreChannelsSwapped ())
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
	m_MIDIKeyboard.Process (bPlugAndPlayUpdated);

	m_Keyboard.Process (bPlugAndPlayUpdated);

	if (m_bUseSerial)
	{
		m_SerialMIDI.Process ();
	}
}

void CMiniSynthesizer::SetPatch (CPatch *pPatch)
{
	assert (pPatch != 0);

	m_VoiceManager.SetPatch (pPatch);

	m_nVolumeLevel =
#if defined (USE_I2S) || defined (USE_HDMI)
			   m_nMaxLevel
#else
			   m_nMaxLevel/2
#endif
			 * powf (pPatch->GetParameter (SynthVolume) / 100.0, 3.3f); // apply some curve
}

void CMiniSynthesizer::NoteOn (u8 ucKeyNumber, u8 ucVelocity)
{
	// apply velocity curve
	assert (m_pConfig != 0);
	ucVelocity = m_pConfig->MapVelocity (ucVelocity);

	m_VoiceManager.NoteOn (ucKeyNumber, ucVelocity);
}

void CMiniSynthesizer::NoteOff (u8 ucKeyNumber)
{
	m_VoiceManager.NoteOff (ucKeyNumber);
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

	CPatch *pPatch = m_pConfig->GetActivePatch ();
	assert (pPatch != 0);

	pPatch->SetMIDIParameter (Parameter, ucValue);
	SetPatch (pPatch);

	m_nConfigRevisionWrite++;
}

void CMiniSynthesizer::ProgramChange (u8 ucProgram)
{
	assert (m_pConfig != 0);

	if (ucProgram < PATCHES)
	{
		m_pConfig->SetActivePatchNumber (ucProgram);
		SetPatch (m_pConfig->GetActivePatch ());
		m_nConfigRevisionWrite++;
	}
}

unsigned CMiniSynthesizer::GetChunk (u32 *pBuffer, unsigned nChunkSize)
{
#ifdef SHOW_STATUS
	unsigned nTicks = CTimer::GetClockTicks ();
#endif

	unsigned nResult = nChunkSize;

#ifdef USE_HDMI
	unsigned nFrame = 0;
#endif
	for (; nChunkSize > 0; nChunkSize -= 2)		// fill the whole buffer
	{
		m_VoiceManager.NextSample ();

		float fLevelLeft = m_VoiceManager.GetOutputLevelLeft ();
		int nLevelLeft = (int) (fLevelLeft*m_nVolumeLevel + m_nNullLevel);
		if (nLevelLeft > (int) m_nMaxLevel)
		{
			nLevelLeft = m_nMaxLevel;
		}
#if defined (USE_I2S) || defined (USE_HDMI)
		else if (nLevelLeft < m_nMinLevel)
		{
			nLevelLeft = m_nMinLevel;
		}
#else
		else if (nLevelLeft < 0)
		{
			nLevelLeft = 0;
		}
#endif

		float fLevelRight = m_VoiceManager.GetOutputLevelRight ();
		int nLevelRight = (int) (fLevelRight*m_nVolumeLevel + m_nNullLevel);
		if (nLevelRight > (int) m_nMaxLevel)
		{
			nLevelRight = m_nMaxLevel;
		}
#if defined (USE_I2S) || defined (USE_HDMI)
		else if (nLevelRight < m_nMinLevel)
		{
			nLevelRight = m_nMinLevel;
		}
#else
		else if (nLevelRight < 0)
		{
			nLevelRight = 0;
		}
#endif

		u32 nSampleLeft = (u32) nLevelLeft;
		u32 nSampleRight = (u32) nLevelRight;

#ifdef USE_HDMI
		nSampleLeft = ConvertIEC958Sample (nSampleLeft, nFrame);
		nSampleRight = ConvertIEC958Sample (nSampleRight, nFrame);

		if (++nFrame == IEC958_FRAMES_PER_BLOCK)
		{
			nFrame = 0;
		}
#endif

		// for 2 stereo channels
		if (!m_bChannelsSwapped)
		{
			*pBuffer++ = nSampleLeft;
			*pBuffer++ = nSampleRight;
		}
		else
		{
			*pBuffer++ = nSampleLeft;
			*pBuffer++ = nSampleRight;
		}
	}

#ifdef SHOW_STATUS
	nTicks = CTimer::GetClockTicks () - nTicks;
	if (nTicks > m_nMaxDelayTicks)
	{
		m_nMaxDelayTicks = nTicks;
	}
#endif

	return nResult;
}

#ifdef SHOW_STATUS

const char *CMiniSynthesizer::GetStatus (void)
{
	m_Status.Format ("%u ms", m_nMaxDelayTicks * 1000 / CLOCKHZ);

	return m_Status;
}

#endif
