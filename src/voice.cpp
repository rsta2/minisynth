//
// voice.cpp
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
#include "voice.h"
#include <assert.h>

// See: http://www.deimos.ca/notefreqs/
static const float KeyFrequency[/* MIDI key number */] =
{
	8.17580, 8.66196, 9.17702, 9.72272, 10.3009, 10.9134, 11.5623, 12.2499, 12.9783, 13.7500,
	14.5676, 15.4339, 16.3516, 17.3239, 18.3540, 19.4454, 20.6017, 21.8268, 23.1247, 24.4997,
	25.9565, 27.5000, 29.1352, 30.8677, 32.7032, 34.6478, 36.7081, 38.8909, 41.2034, 43.6535,
	46.2493, 48.9994, 51.9131, 55.0000, 58.2705, 61.7354, 65.4064, 69.2957, 73.4162, 77.7817,
	82.4069, 87.3071, 92.4986, 97.9989, 103.826, 110.000, 116.541, 123.471, 130.813, 138.591,
	146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220.000, 233.082, 246.942,
	261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440.000,
	466.164, 493.883, 523.251, 554.365, 587.330, 622.254, 659.255, 698.456, 739.989, 783.991,
	830.609, 880.000, 932.328, 987.767, 1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91,
	1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53, 2093.00, 2217.46, 2349.32, 2489.02,
	2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07, 4186.01, 4434.92,
	4698.64, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040.00, 7458.62, 7902.13,
	8372.02, 8869.84, 9397.27, 9956.06, 10548.1, 11175.3, 11839.8, 12543.9
};

CVoice::CVoice (void)
:	m_VCO (&m_LFO_VCO),
	m_VCO2 (&m_LFO_VCO),
	m_VCO_Mixer (&m_VCO, &m_VCO2),
	m_VCF (&m_VCO_Mixer, &m_LFO_VCF, &m_EG_VCF),
	m_VCA (&m_VCF, &m_LFO_VCA, &m_EG_VCA),
	m_ucKeyNumber (KEY_NUMBER_NONE)
{
}

CVoice::~CVoice (void)
{
}

void CVoice::SetPatch (CPatch *pPatch)
{
	assert (pPatch != 0);

	// VCO
	m_LFO_VCO.SetWaveform ((TWaveform) pPatch->GetParameter (LFOVCOWaveform));
	m_LFO_VCO.SetFrequency (pPatch->GetParameter (LFOVCOFrequency));

	m_VCO.SetWaveform ((TWaveform) pPatch->GetParameter (VCOWaveform));
	m_VCO.SetModulationVolume (pPatch->GetParameter (VCOModulationVolume) / 100.0);

	m_VCO2.SetWaveform ((TWaveform) pPatch->GetParameter (VCOWaveform));
	m_VCO2.SetModulationVolume (pPatch->GetParameter (VCOModulationVolume) / 100.0);
	m_VCO2.SetDetune (pPatch->GetParameter (VCODetune) / 100.0 - 1.0);

	// VCF
	m_LFO_VCF.SetWaveform ((TWaveform) pPatch->GetParameter (LFOVCFWaveform));
	m_LFO_VCF.SetFrequency (pPatch->GetParameter (LFOVCFFrequency) / 10.0);

	m_VCF.SetCutoffFrequency (pPatch->GetParameter (VCFCutoffFrequency));
	m_VCF.SetResonance (pPatch->GetParameter (VCFResonance));

	m_EG_VCF.SetAttack (pPatch->GetParameter (EGVCFAttack));
	m_EG_VCF.SetDecay (pPatch->GetParameter (EGVCFDecay));
	m_EG_VCF.SetSustain (pPatch->GetParameter (EGVCFSustain) / 100.0);
	m_EG_VCF.SetRelease (pPatch->GetParameter (EGVCFRelease));

	m_VCF.SetModulationVolume (pPatch->GetParameter (VCFModulationVolume) / 100.0);

	// VCA
	m_LFO_VCA.SetWaveform ((TWaveform) pPatch->GetParameter (LFOVCAWaveform));
	m_LFO_VCA.SetFrequency (pPatch->GetParameter (LFOVCAFrequency) / 10.0);

	m_EG_VCA.SetAttack (pPatch->GetParameter (EGVCAAttack));
	m_EG_VCA.SetDecay (pPatch->GetParameter (EGVCADecay));
	m_EG_VCA.SetSustain (pPatch->GetParameter (EGVCASustain) / 100.0);
	m_EG_VCA.SetRelease (pPatch->GetParameter (EGVCARelease));

	m_VCA.SetModulationVolume (pPatch->GetParameter (VCAModulationVolume) / 100.0);
}

void CVoice::NoteOn (u8 ucKeyNumber, u8 ucVelocity)
{
	if (ucKeyNumber < sizeof KeyFrequency / sizeof KeyFrequency[0])
	{
		m_ucKeyNumber = ucKeyNumber;
		m_VCO.SetFrequency (KeyFrequency[m_ucKeyNumber]);
		m_VCO2.SetFrequency (KeyFrequency[m_ucKeyNumber]);

		assert (1 <= ucVelocity && ucVelocity <= 127);
		float fVelocityLevel = ucVelocity / 127.0;
		m_EG_VCF.NoteOn (fVelocityLevel);
		m_EG_VCA.NoteOn (fVelocityLevel);
	}
}

void CVoice::NoteOff (void)
{
	m_EG_VCF.NoteOff ();
	m_EG_VCA.NoteOff ();
}

TVoiceState CVoice::GetState (void) const
{
	switch (m_EG_VCA.GetState ())
	{
	case EnvelopeStateIdle:
		return VoiceStateIdle;

	case EnvelopeStateAttack:
	case EnvelopeStateDecay:
	case EnvelopeStateSustain:
		return VoiceStateActive;

	case EnvelopeStateRelease:
		return VoiceStateRelease;

	default:
		assert (0);
		return VoiceStateActive;
	}
}

u8 CVoice::GetKeyNumber (void) const
{
	return m_EG_VCA.GetState () != EnvelopeStateIdle ? m_ucKeyNumber : KEY_NUMBER_NONE;
}

void CVoice::NextSample (void)
{
	// VCO
	m_LFO_VCO.NextSample ();
	m_VCO.NextSample ();
	m_VCO2.NextSample ();
	m_VCO_Mixer.NextSample ();

	// VCF
	m_LFO_VCF.NextSample ();
	m_EG_VCF.NextSample ();
	m_VCF.NextSample ();

	// VCA
	m_LFO_VCA.NextSample ();
	m_EG_VCA.NextSample ();
	m_VCA.NextSample ();
}

float CVoice::GetOutputLevel (void) const
{
	return m_VCA.GetOutputLevel ();
}
