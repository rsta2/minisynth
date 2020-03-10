//
// reverbmodule.h
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
#ifndef _reverbmodule_h
#define _reverbmodule_h

#include "synthmodule.h"
#include "oscillator.h"

class CReverbAttenuator
{
public:
	CReverbAttenuator (float fDamping);

	void NextSample (float fInputLevel);
	float GetOutputLevel (void) const	{ return m_fOutputLevel; }

private:
	float m_fDamping;

	float m_fMemory;
	float m_fOutputLevel;
};

class CReverbDelay
{
public:
	CReverbDelay (unsigned nDelaySamples,
		      CSynthModule *pLFO = 0, unsigned nExcursion = 0);
	~CReverbDelay (void);

	void NextSample (float fInputLevel);
	float GetOutputLevel (void) const	{ return m_fOutputLevel; }

private:
	unsigned m_nDelaySamples;
	CSynthModule *m_pLFO;
	unsigned m_nExcursion;

	unsigned m_nSize;
	float *m_pMemory;
	unsigned m_nInPtr;
	float m_fOutputLevel;
};

class CReverbDiffuser
{
public:
	CReverbDiffuser (float fDiffusion, unsigned nDelaySamples,
			 CSynthModule *pLFO = 0, unsigned nExcursion = 0);

	void SetDiffusion (float fDiffusion);

	void NextSample (float fInputLevel);
	float GetOutputLevel (void) const	{ return m_fOutputLevel; }

private:
	float m_fDiffusion;
	CReverbDelay m_Delay;
	float m_fOutputLevel;
};

class CReverbModule
{
public:
	CReverbModule (void);

	void SetDecay (float fDecay);
	void SetWetDryRatio (float fWetDryRatio);

	void NextSample (float fInputLevel);
	float GetOutputLevelLeft (void) const	{ return m_fOutputLevelLeft; }
	float GetOutputLevelRight (void) const	{ return m_fOutputLevelRight; }

private:
	const unsigned Excursion = 16;
	const float DecayDiffusion1 = 0.7f;
	const float InputDiffusion1 = 0.75f;
	const float InputDiffusion2 = 0.625f;
	const float Bandwidth = 0.9995f;
	const float Damping = 0.0005f;
	const float LFOFrequency23_24 = 0.5f;
	const float LFOFrequency46_48 = 0.3f;

private:
	float m_fDecay;
	float m_fDecayDiffusion2;
	float m_fWetDryRatio;

	CReverbAttenuator m_BandwidthAttenuator;
	CReverbDiffuser m_InputDiffuser13_14;
	CReverbDiffuser m_InputDiffuser19_20;
	CReverbDiffuser m_InputDiffuser15_16;
	CReverbDiffuser m_InputDiffuser21_22;

	COscillator m_LFO23_24;
	CReverbDiffuser m_DecayDiffuser23_24;
	CReverbDelay m_Delay30;
	CReverbAttenuator m_Attenuator30;
	CReverbDiffuser m_DecayDiffuser31_33;
	CReverbDelay m_Delay39;

	COscillator m_LFO46_48;
	CReverbDiffuser m_DecayDiffuser46_48;
	CReverbDelay m_Delay54;
	CReverbAttenuator m_Attenuator54;
	CReverbDiffuser m_DecayDiffuser55_59;
	CReverbDelay m_Delay63;

	CReverbDelay m_DelayL48_54_1;
	CReverbDelay m_DelayL48_54_2;
	CReverbDelay m_DelayL55_59;
	CReverbDelay m_DelayL59_63;
	CReverbDelay m_DelayL24_30;
	CReverbDelay m_DelayL31_33;
	CReverbDelay m_DelayL33_39;
	float m_fOutputLevelLeft;

	CReverbDelay m_DelayR24_30_1;
	CReverbDelay m_DelayR24_30_2;
	CReverbDelay m_DelayR31_33;
	CReverbDelay m_DelayR33_39;
	CReverbDelay m_DelayR48_54;
	CReverbDelay m_DelayR55_59;
	CReverbDelay m_DelayR59_63;
	float m_fOutputLevelRight;
};

#endif
