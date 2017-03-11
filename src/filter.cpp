//
// filter.cpp
//
// This is based on:
//	Cookbook formulae for audio EQ biquad filter coefficients
//		by Robert Bristow-Johnson  <rbj@audioimagination.com>
//		http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
//	Digital Synth WRA32 5.1.1 by Ryo Ishigaki
//		https://github.com/risgk/digital-synth-wra32/blob/master/vcf.js
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
#include "filter.h"
#include "math.h"
#include "config.h"
#include <assert.h>

#define MAX_FREQ	20000

CFilter::CFilter (CSynthModule *pInput, CSynthModule *pModulator, CSynthModule *pEnvelope)
:	m_pInput (pInput),
	m_pModulator (pModulator),
	m_pEnvelope (pEnvelope),
	m_fCutoffFrequency (80.0),
	m_fResonance (50.0),
	m_fModulationVolume (0.0),
	m_X1 (0.0),
	m_X2 (0.0),
	m_Y0 (0.0),
	m_Y1 (0.0),
	m_Y2 (0.0)
{
}

CFilter::~CFilter (void)
{
	m_pInput = 0;
	m_pModulator = 0;
	m_pEnvelope = 0;
}

void CFilter::SetCutoffFrequency (unsigned nPercent)
{
	assert (nPercent <= 100);
	m_fCutoffFrequency = (float) nPercent;
}

void CFilter::SetResonance (unsigned nPercent)
{
	assert (nPercent <= 100);
	m_fResonance = (float) nPercent;

	// optimizing for speed because "a" is fixed: pow(a, b) == exp(log(a)*b)
	// m_Q = powf (sqrt (2), (m_fResonance - 100.0/5.0) / (100.0/5.0));
#define LOG_SQRT2	0.34657359f
	m_Q = expf (LOG_SQRT2 * (m_fResonance - 100.0/5.0) / (100.0/5.0));
}

void CFilter::SetModulationVolume (float fVolume)
{
	assert (0.0 <= fVolume && fVolume <= 1.0);
	m_fModulationVolume = fVolume;
}

void CFilter::NextSample (void)
{
	assert (m_pModulator != 0);
	float fCutoffFrequency =   m_fCutoffFrequency * (1.0-m_fModulationVolume)
				 + m_pModulator->GetOutputLevel () * m_fModulationVolume;

	assert (m_pEnvelope != 0);
	fCutoffFrequency *= m_pEnvelope->GetOutputLevel ();

	if (fCutoffFrequency < 10)
	{
		fCutoffFrequency = 10;
	}
	else if (fCutoffFrequency > 100)
	{
		fCutoffFrequency = 100;
	}

	CalculateCoefficients (fCutoffFrequency);

	assert (m_pInput != 0);
	float X0 = m_pInput->GetOutputLevel ();

	m_Y0 = (m_B0_B2*X0 + m_B1*m_X1 + m_B0_B2*m_X2 - m_A1*m_Y1 - m_A2*m_Y2) / m_A0;

	m_X2 = m_X1;
	m_Y2 = m_Y1;
	m_X1 = X0;
	m_Y1 = m_Y0;
}

float CFilter::GetOutputLevel (void) const
{
	return m_Y0;
}

void CFilter::CalculateCoefficients (float fCutoffFrequency)
{
	// optimizing for speed because "a" is fixed: pow(a, b) == exp(log(a)*b)
	// float F0 = powf (2.0, (m_fCutoffFrequency-100.0) / 10.0) * MAX_FREQ;
#define LOG_2	0.69314718f
	float F0 = expf (LOG_2 * (fCutoffFrequency-100.0) / 10.0) * MAX_FREQ;

	float W0 = 2.0*PI * F0 / SAMPLE_RATE;
	float Alpha = sinf (W0) / (2.0*m_Q);
	float CosW0 = cosf (W0);

	m_A0 =  1.0 + Alpha;
	m_A1 = -2.0 * CosW0;
	m_A2 =  1.0 - Alpha;
	m_B1 =  1.0 - CosW0;
	m_B0_B2 = m_B1 / 2.0;		// coefficients B0 and B2 are equal
}
