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
#include <circle/logger.h>
#include <assert.h>

static const char FromMiniSynth[] = "synth";

CMiniSynthesizer::CMiniSynthesizer (CInterruptSystem *pInterrupt)
:	CPWMSoundDevice2 (pInterrupt, SAMPLE_RATE),
	m_Keyboard (this),
	m_nNullLevel ((GetRange ()-1) / 2),
	m_nVolumeLevel (0.0)
{
	for (unsigned i = 0; i < VOICES; i++)
	{
		m_pVoice[i] = new CVoice ();
		assert (m_pVoice[i] != 0);
	}
}

CMiniSynthesizer::~CMiniSynthesizer (void)
{
	for (unsigned i = 0; i < VOICES; i++)
	{
		delete m_pVoice[i];
		m_pVoice[i] = 0;
	}
}

boolean CMiniSynthesizer::Initialize (void)
{
	if (!m_Keyboard.Initialize ())
	{
		CLogger::Get ()->Write (FromMiniSynth, LogWarning, "PC Keyboard not found");
	}

	return TRUE;
}


void CMiniSynthesizer::SetPatch (CPatch *pPatch)
{
	assert (pPatch != 0);

	for (unsigned i = 0; i < VOICES; i++)
	{
		assert (m_pVoice[i] != 0);
		m_pVoice[i]->SetPatch (pPatch);
	}

	m_nVolumeLevel = (GetRange ()-1) / 2 * pPatch->GetParameter (SynthVolume) / 100;
}

void CMiniSynthesizer::NoteOn (u8 ucKeyNumber, u8 ucVelocity)
{
	// find an unused voice or a releasing voice otherwise
	CVoice *pReleaseVoice = 0;
	unsigned i;
	for (i = 0; i < VOICES; i++)
	{
		assert (m_pVoice[i] != 0);
		TVoiceState State = m_pVoice[i]->GetState ();
		if (State == VoiceStateIdle)
		{
			break;
		}

		if (   State == VoiceStateRelease
		    && pReleaseVoice == 0)
		{
			pReleaseVoice = m_pVoice[i];
		}
	}

	if (i < VOICES)
	{
		assert (m_pVoice[i] != 0);
		m_pVoice[i]->NoteOn (ucKeyNumber, ucVelocity);
	}
	else if (pReleaseVoice != 0)
	{
		pReleaseVoice->NoteOn (ucKeyNumber, ucVelocity);
	}
}

void CMiniSynthesizer::NoteOff (u8 ucKeyNumber)
{
	// find the voice used for this key
	unsigned i;
	for (i = 0; i < VOICES; i++)
	{
		assert (m_pVoice[i] != 0);
		if (m_pVoice[i]->GetKeyNumber () == ucKeyNumber)
		{
			break;
		}
	}

	if (i < VOICES)
	{
		assert (m_pVoice[i] != 0);
		m_pVoice[i]->NoteOff ();
	}
}

unsigned CMiniSynthesizer::GetChunk (u32 *pBuffer, unsigned nChunkSize)
{
	unsigned nResult = nChunkSize;

	for (; nChunkSize > 0; nChunkSize -= 2)		// fill the whole buffer
	{
		float fLevel = 0.0;

		for (unsigned i = 0; i < VOICES; i++)
		{
			assert (m_pVoice[i] != 0);
			if (m_pVoice[i]->GetState () != VoiceStateIdle)
			{
				m_pVoice[i]->NextSample ();

				fLevel += m_pVoice[i]->GetOutputLevel ();
			}
		}

		fLevel /= VOICES;

		if (fLevel > 1.0)
		{
			fLevel = 1.0;
		}
		else if (fLevel < -1.0)
		{
			fLevel = -1.0;
		}

		unsigned nLevel = (unsigned) (fLevel*m_nVolumeLevel + m_nNullLevel + 0.5);

		// for 2 stereo channels
		*pBuffer++ = nLevel;
		*pBuffer++ = nLevel;
	}

	return nResult;
}
