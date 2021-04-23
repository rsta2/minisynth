//
// voicemanager.h
//
// Manages the polyphonic voices and available CPU cores
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
#ifndef _voicemanager_h
#define _voicemanager_h

#include <circle/multicore.h>
#include <circle/memory.h>
#include <circle/types.h>
#include "patch.h"
#include "notestack.h"
#include "voice.h"
#include "reverbmodule.h"
#include "config.h"

#ifdef ARM_ALLOW_MULTI_CORE
	#define VOICES		(VOICES_PER_CORE * CORES)
#else
	#define VOICES		VOICES_PER_CORE
#endif

#ifdef ARM_ALLOW_MULTI_CORE

enum TCoreStatus
{
	CoreStatusInit,
	CoreStatusIdle,
	CoreStatusBusy,
	CoreStatusExit,
	CoreStatusUnknown
};

#endif

// Except Run() and ProcessVoices() everything herein runs on core 0.
// m_CoreStatus[] is used to synchronize the secondary cores from core 0. Normally
// m_CoreStatus[] is CoreStatusIdle for all secondary cores and they are spinning
// to wait until this status changes to CoreStatusBusy. This is triggered in
// NextSample(), where the major workload is done. Each core processes the same
// number of voices by calling ProcessVoices() and sums up their current output level
// to m_fOutputLevel[]. These levels are mixed together when GetOutputLevel() gets
// called from CMiniSynthesizer::GetChunk(). When the secondary cores have done
// their work they go back to CoreStatusIdle to be triggered again.

class CVoiceManager
#ifdef ARM_ALLOW_MULTI_CORE
	: public CMultiCoreSupport
#endif
{
public:
	CVoiceManager (CMemorySystem *pMemorySystem);
	~CVoiceManager (void);

	boolean Initialize (void);

#ifdef ARM_ALLOW_MULTI_CORE
	void Run (unsigned nCore);			// secondary core entry
#endif

	void SetPatch (CPatch *pPatch);

	void NoteOn (u8 ucKeyNumber, u8 ucVelocity);	// MIDI key number and velocity
	void NoteOff (u8 ucKeyNumber);

	void NextSample (void);
	float GetOutputLevelLeft (void) const;
	float GetOutputLevelRight (void) const;

private:
	float ProcessVoices (unsigned nFirst, unsigned nLast);

private:
	CNoteStack m_NoteStack;

	CVoice *m_pVoice[VOICES];

	unsigned m_nActiveVoices;
	unsigned m_nLastNoteOnVoice;

#ifdef ARM_ALLOW_MULTI_CORE
	volatile TCoreStatus m_CoreStatus[CORES];

	volatile float m_fOutputLevel[CORES];
#endif

	CReverbModule m_ReverbModule;
};

#endif
