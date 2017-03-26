//
// minisynth.cpp
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
#include "minisynth.h"
#include "math.h"
#include <circle/logger.h>
#include <assert.h>

static const char FromMiniSynth[] = "synth";

CMiniSynthesizer::CMiniSynthesizer (CSynthConfig *pConfig,
				    CInterruptSystem *pInterrupt, CMemorySystem *pMemorySystem)
:	CPWMSoundBaseDevice (pInterrupt, SAMPLE_RATE),
	m_pConfig (pConfig),
	m_MIDIKeyboard (this),
	m_Keyboard (this),
	m_VoiceManager (pMemorySystem),
	m_nMaxLevel (GetRange ()-1),
	m_nNullLevel (m_nMaxLevel / 2),
	m_nVolumeLevel (0)
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

	CLogger::Get ()->Write (FromMiniSynth, LogWarning, "Keyboard not found");

	return FALSE;
}

void CMiniSynthesizer::SetPatch (CPatch *pPatch)
{
	assert (pPatch != 0);

	m_VoiceManager.SetPatch (pPatch);

	m_nVolumeLevel =   m_nMaxLevel/2
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

unsigned CMiniSynthesizer::GetChunk (u32 *pBuffer, unsigned nChunkSize)
{
	unsigned nResult = nChunkSize;

	for (; nChunkSize > 0; nChunkSize -= 2)		// fill the whole buffer
	{
		m_VoiceManager.NextSample ();
		float fLevel = m_VoiceManager.GetOutputLevel ();

		int nLevel = (int) (fLevel*m_nVolumeLevel + m_nNullLevel);

		if (nLevel > (int) m_nMaxLevel)
		{
			nLevel = m_nMaxLevel;
		}
		else if (nLevel < 0)
		{
			nLevel = 0;
		}

		// for 2 stereo channels
		*pBuffer++ = (unsigned) nLevel;
		*pBuffer++ = (unsigned) nLevel;
	}

	return nResult;
}
