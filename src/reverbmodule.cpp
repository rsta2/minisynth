//
// reverbmodule.cpp
//
// See:	Jon Dattorro: Effect Design, Part 1: Reverberator and Other Filters
//	CCRMA, Stanford University, Stanford, CA, USA; 1997
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2020  R. Stange <rsta2@o2online.de>
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
#include "reverbmodule.h"
#include <math.h>

CReverbAttenuator::CReverbAttenuator (float fDamping)
:	m_fDamping (fDamping),
	m_fMemory (0.0f),
	m_fOutputLevel (0.0f)
{
}

void CReverbAttenuator::NextSample (float fInputLevel)
{
	m_fOutputLevel = fInputLevel*(1.0f-m_fDamping) + m_fMemory*m_fDamping;

	m_fMemory = fInputLevel;
}

CReverbDelay::CReverbDelay (unsigned nDelaySamples, CSynthModule *pLFO, unsigned nExcursion)
:	m_nDelaySamples (nDelaySamples),
	m_pLFO (pLFO),
	m_nExcursion (nExcursion),
	m_nSize (nDelaySamples+nExcursion+1),
	m_pMemory (new float[m_nSize]),
	m_nInPtr (m_nSize-1),
	m_fOutputLevel (0.0f)
{
	for (unsigned i = 0; i < m_nSize; i++)
	{
		m_pMemory[i] = 0.0f;
	}
}

CReverbDelay::~CReverbDelay (void)
{
	delete [] m_pMemory;
}

void CReverbDelay::NextSample (float fInputLevel)
{
	unsigned nDelay = m_nDelaySamples;
	if (m_pLFO != 0)
	{
		nDelay += m_pLFO->GetOutputLevel ()*m_nExcursion;
	}

	unsigned nOutPtr = m_nInPtr-nDelay;
	if (nOutPtr >= m_nSize)
	{
		nOutPtr += m_nSize;
	}

	m_fOutputLevel = m_pMemory[nOutPtr];

	m_pMemory[m_nInPtr++] = fInputLevel;
	if (m_nInPtr == m_nSize)
	{
		m_nInPtr = 0;
	}
}

CReverbDiffuser::CReverbDiffuser (float fDiffusion, unsigned nDelaySamples,
				  CSynthModule *pLFO, unsigned nExcursion)
:	m_fDiffusion (fDiffusion),
	m_Delay (nDelaySamples, pLFO, nExcursion),
	m_fOutputLevel (0.0f)
{
}

void CReverbDiffuser::SetDiffusion (float fDiffusion)
{
	m_fDiffusion = fDiffusion;
}

void CReverbDiffuser::NextSample (float fInputLevel)
{
	float fTemp = fInputLevel - m_Delay.GetOutputLevel ()*m_fDiffusion;

	m_Delay.NextSample (fTemp);

	m_fOutputLevel = fTemp*m_fDiffusion + m_Delay.GetOutputLevel ();
}

CReverbModule::CReverbModule (void)
:	m_fDecay (0.5f),
	m_fDecayDiffusion2 (0.5f),
	m_fWetDryRatio (0.25f),

	m_BandwidthAttenuator (1.0f-Bandwidth),
	m_InputDiffuser13_14 (InputDiffusion1, 142),
	m_InputDiffuser19_20 (InputDiffusion1, 107),
	m_InputDiffuser15_16 (InputDiffusion2, 379),
	m_InputDiffuser21_22 (InputDiffusion2, 277),

	m_DecayDiffuser23_24 (DecayDiffusion1, 672, &m_LFO23_24, Excursion),
	m_Delay30 (4453),
	m_Attenuator30 (Damping),
	m_DecayDiffuser31_33 (m_fDecayDiffusion2, 1800),
	m_Delay39 (3720),

	m_DecayDiffuser46_48 (DecayDiffusion1, 908, &m_LFO46_48, Excursion),
	m_Delay54 (4217),
	m_Attenuator54 (Damping),
	m_DecayDiffuser55_59 (m_fDecayDiffusion2, 2656),
	m_Delay63 (3163),

	m_DelayL48_54_1 (266),
	m_DelayL48_54_2 (2974),
	m_DelayL55_59 (1913),
	m_DelayL59_63 (1996),
	m_DelayL24_30 (1990),
	m_DelayL31_33 (187),
	m_DelayL33_39 (1066),
	m_fOutputLevelLeft (0.0f),

	m_DelayR24_30_1 (353),
	m_DelayR24_30_2 (3627),
	m_DelayR31_33 (1228),
	m_DelayR33_39 (2673),
	m_DelayR48_54 (2111),
	m_DelayR55_59 (335),
	m_DelayR59_63 (121),
	m_fOutputLevelRight (0.0f)
{
	m_LFO23_24.SetWaveform (WaveformSine);
	m_LFO23_24.SetFrequency (LFOFrequency23_24);

	m_LFO46_48.SetWaveform (WaveformSine);
	m_LFO46_48.SetFrequency (LFOFrequency46_48);
}

void CReverbModule::SetDecay (float fDecay)
{
	m_fDecay = fDecay;

	m_fDecayDiffusion2 = ceilf (floorf ((m_fDecay + 0.15f) * 4.0f) / 2.0f) / 2.0f;

	m_DecayDiffuser31_33.SetDiffusion (m_fDecayDiffusion2);
	m_DecayDiffuser55_59.SetDiffusion (m_fDecayDiffusion2);
}

void CReverbModule::SetWetDryRatio (float fWetDryRatio)
{
	m_fWetDryRatio = fWetDryRatio;
}

void CReverbModule::NextSample (float fInputLevel)
{
	m_BandwidthAttenuator.NextSample (fInputLevel);

	m_InputDiffuser13_14.NextSample (m_BandwidthAttenuator.GetOutputLevel ());
	m_InputDiffuser19_20.NextSample (m_InputDiffuser13_14.GetOutputLevel ());
	m_InputDiffuser15_16.NextSample (m_InputDiffuser19_20.GetOutputLevel ());
	m_InputDiffuser21_22.NextSample (m_InputDiffuser15_16.GetOutputLevel ());

	m_LFO23_24.NextSample ();
	m_DecayDiffuser23_24.NextSample (  m_InputDiffuser21_22.GetOutputLevel ()
					 + m_Delay63.GetOutputLevel ()*m_fDecay);
	m_Delay30.NextSample (m_DecayDiffuser23_24.GetOutputLevel ());
	m_Attenuator30.NextSample (m_Delay30.GetOutputLevel ());
	m_DecayDiffuser31_33.NextSample (m_Attenuator30.GetOutputLevel ()*m_fDecay);
	m_Delay39.NextSample (m_DecayDiffuser31_33.GetOutputLevel ());

	m_LFO46_48.NextSample ();
	m_DecayDiffuser46_48.NextSample (  m_InputDiffuser21_22.GetOutputLevel ()
					 + m_Delay39.GetOutputLevel ()*m_fDecay);
	m_Delay54.NextSample (m_DecayDiffuser46_48.GetOutputLevel ());
	m_Attenuator54.NextSample (m_Delay54.GetOutputLevel ());
	m_DecayDiffuser55_59.NextSample (m_Attenuator54.GetOutputLevel ()*m_fDecay);
	m_Delay63.NextSample (m_DecayDiffuser55_59.GetOutputLevel ());

	m_DelayL48_54_1.NextSample (m_DecayDiffuser46_48.GetOutputLevel ());
	m_DelayL48_54_2.NextSample (m_Delay54.GetOutputLevel ());
	m_DelayL55_59.NextSample (m_DecayDiffuser55_59.GetOutputLevel ());
	m_DelayL59_63.NextSample (m_Delay63.GetOutputLevel ());
	m_DelayL24_30.NextSample (m_DecayDiffuser23_24.GetOutputLevel ());
	m_DelayL31_33.NextSample (m_DecayDiffuser31_33.GetOutputLevel ());
	m_DelayL33_39.NextSample (m_Delay39.GetOutputLevel ());

	float fAccu;
	fAccu  = 0.6f * m_DelayL48_54_1.GetOutputLevel ();
	fAccu += 0.6f * m_DelayL48_54_2.GetOutputLevel ();
	fAccu -= 0.6f * m_DelayL55_59.GetOutputLevel ();
	fAccu += 0.6f * m_DelayL59_63.GetOutputLevel ();
	fAccu -= 0.6f * m_DelayL24_30.GetOutputLevel ();
	fAccu -= 0.6f * m_DelayL31_33.GetOutputLevel ();
	fAccu -= 0.6f * m_DelayL33_39.GetOutputLevel ();
	m_fOutputLevelLeft = fInputLevel*(1.0f-m_fWetDryRatio) + fAccu*m_fWetDryRatio;

	m_DelayR24_30_1.NextSample (m_DecayDiffuser23_24.GetOutputLevel ());
	m_DelayR24_30_2.NextSample (m_Delay30.GetOutputLevel ());
	m_DelayR31_33.NextSample (m_DecayDiffuser31_33.GetOutputLevel ());
	m_DelayR33_39.NextSample (m_Delay39.GetOutputLevel ());
	m_DelayR48_54.NextSample (m_DecayDiffuser46_48.GetOutputLevel ());
	m_DelayR55_59.NextSample (m_DecayDiffuser55_59.GetOutputLevel ());
	m_DelayR59_63.NextSample (m_Delay63.GetOutputLevel ());

	fAccu  = 0.6f * m_DelayR24_30_1.GetOutputLevel ();
	fAccu += 0.6f * m_DelayR24_30_2.GetOutputLevel ();
	fAccu -= 0.6f * m_DelayR31_33.GetOutputLevel ();
	fAccu += 0.6f * m_DelayR33_39.GetOutputLevel ();
	fAccu -= 0.6f * m_DelayR48_54.GetOutputLevel ();
	fAccu -= 0.6f * m_DelayR55_59.GetOutputLevel ();
	fAccu -= 0.6f * m_DelayR59_63.GetOutputLevel ();
	m_fOutputLevelRight = fInputLevel*(1.0f-m_fWetDryRatio) + fAccu*m_fWetDryRatio;
}
