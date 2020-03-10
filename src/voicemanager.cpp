//
// voicemanager.cpp
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
#include "voicemanager.h"
#include <assert.h>

CVoiceManager::CVoiceManager (CMemorySystem *pMemorySystem)
:
#ifdef ARM_ALLOW_MULTI_CORE
	CMultiCoreSupport (pMemorySystem),
#endif
	m_nLastNoteOnVoice (VOICES)
{
	for (unsigned i = 0; i < VOICES; i++)
	{
		m_pVoice[i] = new CVoice ();
		assert (m_pVoice[i] != 0);
	}

#ifdef ARM_ALLOW_MULTI_CORE
	for (unsigned nCore = 0; nCore < CORES; nCore++)
	{
		m_CoreStatus[nCore] = CoreStatusInit;

		m_fOutputLevel[nCore] = 0.0;
	}
#endif
}

CVoiceManager::~CVoiceManager (void)
{
#ifdef ARM_ALLOW_MULTI_CORE
	for (unsigned nCore = 1; nCore < CORES; nCore++)
	{
		assert (m_CoreStatus[nCore] == CoreStatusIdle);
		m_CoreStatus[nCore] = CoreStatusExit;

		while (m_CoreStatus[nCore] == CoreStatusExit)
		{
			// just wait
		}
	}
#endif

	for (unsigned i = 0; i < VOICES; i++)
	{
		delete m_pVoice[i];
		m_pVoice[i] = 0;
	}
}

boolean CVoiceManager::Initialize (void)
{
#ifdef ARM_ALLOW_MULTI_CORE
	if (!CMultiCoreSupport::Initialize ())
	{
		return FALSE;
	}

	// wait for secondary cores to be ready
	for (unsigned nCore = 1; nCore < CORES; nCore++)
	{
		while (m_CoreStatus[nCore] != CoreStatusIdle)
		{
			// just wait
		}
	}
#endif

	return TRUE;
}

#ifdef ARM_ALLOW_MULTI_CORE

void CVoiceManager::Run (unsigned nCore)	// runs on secondary cores
{
	assert (1 <= nCore && nCore < CORES);
	unsigned nFirstVoice = nCore * VOICES_PER_CORE;
	unsigned nLastVoice  = nFirstVoice + VOICES_PER_CORE-1;

	while (1)
	{
		m_CoreStatus[nCore] = CoreStatusIdle;			// ready to be kicked
		while (m_CoreStatus[nCore] == CoreStatusIdle)
		{
			// just wait
		}

		if (m_CoreStatus[nCore] == CoreStatusExit)
		{
			m_CoreStatus[nCore] = CoreStatusUnknown;

			return;
		}

		assert (m_CoreStatus[nCore] == CoreStatusBusy);

		m_fOutputLevel[nCore] = ProcessVoices (nFirstVoice, nLastVoice);
	}
}

#endif

void CVoiceManager::SetPatch (CPatch *pPatch)
{
	assert (pPatch != 0);

	for (unsigned i = 0; i < VOICES; i++)
	{
		assert (m_pVoice[i] != 0);
		m_pVoice[i]->SetPatch (pPatch);
	}

	m_ReverbModule.SetDecay (pPatch->GetParameter (ReverbDecay) / 100.0f);
	m_ReverbModule.SetWetDryRatio (pPatch->GetParameter (ReverbVolume) / 100.0f);
}

void CVoiceManager::NoteOn (u8 ucKeyNumber, u8 ucVelocity)
{
	// find the voice which is currently playing this key
	unsigned i;
	for (i = 0; i < VOICES; i++)
	{
		assert (m_pVoice[i] != 0);
		if (m_pVoice[i]->GetKeyNumber () == ucKeyNumber)
		{
			break;
		}
	}

	if (i >= VOICES)
	{
		// otherwise find a free voice
		for (i = 0; i < VOICES; i++)
		{
			assert (m_pVoice[i] != 0);
			if (m_pVoice[i]->GetState () == VoiceStateIdle)
			{
				break;
			}
		}
	}

	if (i < VOICES)
	{
		assert (m_pVoice[i] != 0);
		m_pVoice[i]->NoteOn (ucKeyNumber, ucVelocity);

		m_nLastNoteOnVoice = i;
	}
#ifdef LAST_NOTE_PRIORITY
	else
	{
		assert (m_nLastNoteOnVoice < VOICES);
		assert (m_pVoice[m_nLastNoteOnVoice] != 0);
		m_pVoice[m_nLastNoteOnVoice]->NoteOn (ucKeyNumber, ucVelocity);
	}
#endif
}

void CVoiceManager::NoteOff (u8 ucKeyNumber)
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

void CVoiceManager::NextSample (void)		// runs on core 0
{
#ifdef ARM_ALLOW_MULTI_CORE
	// kick secondary cores
	for (unsigned nCore = 1; nCore < CORES; nCore++)
	{
		assert (m_CoreStatus[nCore] == CoreStatusIdle);
		m_CoreStatus[nCore] = CoreStatusBusy;
	}

	m_fOutputLevel[0] = ProcessVoices (0, VOICES_PER_CORE-1);

	// wait for secondary cores to complete their work
	for (unsigned nCore = 1; nCore < CORES; nCore++)
	{
		while (m_CoreStatus[nCore] != CoreStatusIdle)
		{
			// just wait
		}
	}

	float fLevel = 0.0;
	for (unsigned nCore = 0; nCore < CORES; nCore++)
	{
		fLevel += m_fOutputLevel[nCore];
	}

	m_ReverbModule.NextSample (fLevel);
#else
	m_ReverbModule.NextSample (ProcessVoices (0, VOICES-1));
#endif
}

float CVoiceManager::GetOutputLevelLeft (void) const
{
	return m_ReverbModule.GetOutputLevelLeft ();
}

float CVoiceManager::GetOutputLevelRight (void) const
{
	return m_ReverbModule.GetOutputLevelRight ();
}

float CVoiceManager::ProcessVoices (unsigned nFirst, unsigned nLast)
{
	float fLevel = 0.0;
	for (unsigned i = nFirst; i <= nLast; i++)
	{
		assert (m_pVoice[i] != 0);
		if (m_pVoice[i]->GetState () != VoiceStateIdle)
		{
			m_pVoice[i]->NextSample ();

			fLevel += m_pVoice[i]->GetOutputLevel ();
		}
	}

	return fLevel;
}
