//
// minisynth.cpp
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
#include "minisynth.h"
#include "math.h"
#include "config.h"
#include <circle/logger.h>
#include <assert.h>

static const char FromMiniSynth[] = "synth";

CMiniSynthesizer::CMiniSynthesizer (CSynthConfig *pConfig,
				    CInterruptSystem *pInterrupt, CMemorySystem *pMemorySystem)
#ifdef USE_I2S
:	CI2SSoundBaseDevice (pInterrupt, SAMPLE_RATE, 2048),
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
#ifdef USE_I2S
	m_nMinLevel (GetRangeMin ()+1),
	m_nMaxLevel (GetRangeMax ()-1),
	m_nNullLevel (0),
#else
	m_nMaxLevel (GetRange ()-1),
	m_nNullLevel (m_nMaxLevel / 2),
#endif
	m_nVolumeLevel (0),
	m_bChannelsSwapped (AreChannelsSwapped ())
{
}

CMiniSynthesizer::~CMiniSynthesizer (void)
{
}

boolean CMiniSynthesizer::Initialize (void)
{
	if (m_MIDIKeyboard.Initialize ())
	{
		return m_VoiceManager.Initialize ();
	}

	if (m_Keyboard.Initialize ())
	{
		return m_VoiceManager.Initialize ();
	}

	if (m_SerialMIDI.Initialize ())
	{
		CLogger::Get ()->Write (FromMiniSynth, LogNotice, "Using serial MIDI interface");

		m_bUseSerial = TRUE;

		return m_VoiceManager.Initialize ();
	}

	CLogger::Get ()->Write (FromMiniSynth, LogWarning, "Keyboard not found");

	return FALSE;
}

void CMiniSynthesizer::Process (void)
{
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
#ifdef USE_I2S
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
	TSynthParameter Parameter;

	switch (ucFunction)
	{
	case 7:		Parameter = SynthVolume;		break;
	case 71:	Parameter = VCFResonance;		break;
	case 74:	Parameter = VCFCutoffFrequency;		break;

	default:
		return;
	}

	assert (m_pConfig != 0);
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
	unsigned nResult = nChunkSize;

	for (; nChunkSize > 0; nChunkSize -= 2)		// fill the whole buffer
	{
		m_VoiceManager.NextSample ();

		float fLevelLeft = m_VoiceManager.GetOutputLevelLeft ();
		int nLevelLeft = (int) (fLevelLeft*m_nVolumeLevel + m_nNullLevel);
		if (nLevelLeft > (int) m_nMaxLevel)
		{
			nLevelLeft = m_nMaxLevel;
		}
#ifdef USE_I2S
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
#ifdef USE_I2S
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

	return nResult;
}
