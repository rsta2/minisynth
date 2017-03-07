//
// voice.cpp
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
#include "voice.h"
#include <assert.h>

// See: https://en.wikipedia.org/wiki/Piano_key_frequencies
static const float KeyFrequency[/* MIDI key number */] =
{
	0.0,	// not used

	// 1-88 (A0-C8)
	27.5000, 29.1352, 30.8677, 32.7032, 34.6478, 36.7081, 38.8909, 41.2034, 43.6535, 46.2493,
	48.9994, 51.9131, 55.0000, 58.2705, 61.7354, 65.4064, 69.2957, 73.4162, 77.7817, 82.4069,
	87.3071, 92.4986, 97.9989, 103.826, 110.000, 116.541, 123.471, 130.813, 138.591, 146.832,
	155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220.000, 233.082, 246.942, 261.626,
	277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440.000, 466.164,
	493.883, 523.251, 554.365, 587.330, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609,
	880.000, 932.328, 987.767, 1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98,
	1567.98, 1661.22, 1760.00, 1864.66, 1975.53, 2093.00, 2217.46, 2349.32, 2489.02, 2637.02,
	2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07, 4186.01,

	// 89-97 (C0-G#0)
	16.3516, 17.3239, 18.3540, 19.4454, 20.6017, 21.8268, 23.1247, 24.4997, 25.9565,

	// 98-102 (C#8-F8)
	4434.92, 4698.64, 4978.03, 5274.04, 5587.65
};

CVoice::CVoice (void)
:	m_VCO (&m_LFO_VCO),
	m_VCA (&m_VCO, &m_LFO_VCA, &m_EG_VCA),
	m_ucKeyNumber (0)
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
	m_LFO_VCO.SetPulseWidth (pPatch->GetParameter (LFOVCOPulseWidth) / 100.0);

	m_VCO.SetWaveform ((TWaveform) pPatch->GetParameter (VCOWaveform));
	m_VCO.SetPulseWidth (pPatch->GetParameter (VCOPulseWidth) / 100.0);
	m_VCO.SetModulationFrequency (pPatch->GetParameter (VCOModulationFrequency));

	// VCA
	m_LFO_VCA.SetWaveform ((TWaveform) pPatch->GetParameter (LFOVCAWaveform));
	m_LFO_VCA.SetFrequency (pPatch->GetParameter (LFOVCAFrequency));
	m_LFO_VCA.SetPulseWidth (pPatch->GetParameter (LFOVCAPulseWidth) / 100.0);

	m_EG_VCA.SetAttack (pPatch->GetParameter (EGVCAAttack));
	m_EG_VCA.SetDecay (pPatch->GetParameter (EGVCADecay));
	m_EG_VCA.SetSustain (pPatch->GetParameter (EGVCASustain) / 100.0);
	m_EG_VCA.SetRelease (pPatch->GetParameter (EGVCARelease));

	m_VCA.SetModulationVolume (pPatch->GetParameter (VCAModulationVolume) / 100.0);
}

void CVoice::NoteOn (u8 ucKeyNumber, u8 ucVelocity)
{
	if (0 < ucKeyNumber && ucKeyNumber < sizeof KeyFrequency / sizeof KeyFrequency[0])
	{
		assert (   m_ucKeyNumber == 0
		        || m_ucKeyNumber == ucKeyNumber);
		m_ucKeyNumber = ucKeyNumber;
		m_VCO.SetFrequency (KeyFrequency[m_ucKeyNumber]);

		assert (1 <= ucVelocity && ucVelocity <= 127);
		float fVelocityLevel = ucVelocity / 127.0;
		m_EG_VCA.NoteOn (fVelocityLevel);
	}
}

void CVoice::NoteOff (void)
{
	m_EG_VCA.NoteOff ();

	m_ucKeyNumber = 0;
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
	return m_ucKeyNumber;
}

void CVoice::NextSample (void)
{
	// VCO
	m_LFO_VCO.NextSample ();
	m_VCO.NextSample ();

	// VCA
	m_LFO_VCA.NextSample ();
	m_EG_VCA.NextSample ();
	m_VCA.NextSample ();
}

float CVoice::GetOutputLevel (void) const
{
	return m_VCA.GetOutputLevel ();
}
