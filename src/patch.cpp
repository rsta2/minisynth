//
// patch.cpp
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
#include "patch.h"
#include "oscillator.h"
#include <assert.h>

static struct
{
	const char	*pName;
	TParameterType	 Type;
	unsigned	 nMinimum;
	unsigned	 nMaximum;
	unsigned	 nStep;
	unsigned	 nDefault;
	const char	*pHelp;
}
ParameterList[] =		// must match TSynthParameter
{
	// VCO
	{"LFOVCOWaveform", ParameterWaveform, WaveformSine, WaveformPulse25, 1, WaveformSine, "Wave"},
	{"LFOVCOFrequency", ParameterFrequency, 1, 35, 1, 20, "Rate"},

	{"VCOWaveform", ParameterWaveform, WaveformSine, WaveformUnknown-1, 1, WaveformSquare, "Wave"},
	{"VCOModulationVolume", ParameterPercent, 0, 100, 10, 0, "Volume"},
	{"VCODetune", ParameterPercent, 0, 200, 2, 100, "Detune"},

	// VCF
	{"LFOVCFWaveform", ParameterWaveform, WaveformSine, WaveformPulse25, 1, WaveformSine, "Wave"},
	{"LFOVCFFrequency", ParameterFrequencyTenth, 5, 50, 5, 20, "Rate"},

	{"VCFCutoffFrequency", ParameterPercent, 10, 100, 2, 80, "Cutoff"},
	{"VCFResonance", ParameterPercent, 0, 100, 2, 50, "Resonance"},

	{"EGVCFAttack", ParameterTime, 0, 2000, 50, 0, "Attack"},
	{"EGVCFDecay", ParameterTime, 100, 10000, 100, 4000, "Decay"},
	{"EGVCFSustain", ParameterPercent, 0, 100, 10, 100, "Sustain"},
	{"EGVCFRelease", ParameterTime, 0, 5000, 100, 1000, "Release"},

	{"VCFModulationVolume", ParameterPercent, 0, 100, 5, 0, "Volume"},

	// VCA
	{"LFOVCAWaveform", ParameterWaveform, WaveformSine, WaveformPulse25, 1, WaveformSine, "Wave"},
	{"LFOVCAFrequency", ParameterFrequencyTenth, 5, 50, 5, 20, "Rate"},

	{"EGVCAAttack", ParameterTime, 0, 2000, 50, 100, "Attack"},
	{"EGVCADecay", ParameterTime, 100, 10000, 100, 4000, "Decay"},
	{"EGVCASustain", ParameterPercent, 0, 100, 10, 100, "Sustain"},
	{"EGVCARelease", ParameterTime, 0, 5000, 100, 100, "Release"},

	{"VCAModulationVolume", ParameterPercent, 0, 100, 10, 0, "Volume"},

	// Effects
	{"ReverbDecay", ParameterPercent, 0, 50, 5, 20, "Decay"},
	{"ReverbVolume", ParameterPercent, 0, 30, 5, 0, "Volume"},

	// Synth
	{"SynthVolume", ParameterPercent, 0, 100, 10, 50, "Volume"}
};

static struct
{
	const char	*pName;
	unsigned	 nMaxLength;
	boolean		 bUppercase;
	const char	*pHelp;
}
PropertyList[] =		// must match TPatchProperty
{
	{"Name", 10, TRUE, "Patch name"},
	{"Author", 20, FALSE, "Patch author"},
	{"Comment", 40, FALSE, "Any comment"}
};

CPatch::CPatch (const char *pFileName, FATFS *pFileSystem)
:	m_Properties (pFileName, pFileSystem)
{
	for (unsigned i = 0; i < SynthParameterUnknown; i++)
	{
		m_pParameter[i] = new CParameter (ParameterList[i].pName,
						  ParameterList[i].Type,
						  ParameterList[i].nMinimum,
						  ParameterList[i].nMaximum,
						  ParameterList[i].nStep,
						  ParameterList[i].nDefault,
						  ParameterList[i].pHelp);
		assert (m_pParameter[i] != 0);
	}
}

CPatch::~CPatch (void)
{
	for (unsigned i = 0; i < SynthParameterUnknown; i++)
	{
		delete m_pParameter[i];
		m_pParameter[i] = 0;
	}
}

boolean CPatch::Load (void)
{
	boolean bResult = m_Properties.Load ();
	if (!bResult)
	{
		m_Properties.RemoveAll ();
	}

	for (unsigned i = 0; i < PatchPropertyUnknown; i++)
	{
		m_PropertyString[i] = m_Properties.GetString (PropertyList[i].pName, "");
	}

	for (unsigned i = 0; i < SynthParameterUnknown; i++)
	{
		assert (m_pParameter[i] != 0);
		m_pParameter[i]->Set (m_Properties.GetNumber (m_pParameter[i]->GetName (),
							      m_pParameter[i]->GetDefault ()));
	}

	if (m_Properties.GetNumber ("Version", 1) == 1)		// correct v1 parameters
	{
		// decrease VCF cutoff frequency, because v1 VCF LFO depth had this influence
		m_pParameter[VCFCutoffFrequency]->Set (  m_pParameter[VCFCutoffFrequency]->Get ()
						       - m_pParameter[VCFModulationVolume]->Get ());
		// reset VCF LFO frequency and depth
		m_pParameter[LFOVCFFrequency]->Set (ParameterList[LFOVCFFrequency].nDefault);
		m_pParameter[VCFModulationVolume]->Set (ParameterList[VCFModulationVolume].nDefault);

		// decrease master volume, because v1 VCA LFO depth had this influence
		m_pParameter[SynthVolume]->Set (  m_pParameter[SynthVolume]->Get ()
						- m_pParameter[VCAModulationVolume]->Get ());
		// reset VCA LFO frequency and depth
		m_pParameter[LFOVCAFrequency]->Set (ParameterList[LFOVCAFrequency].nDefault);
		m_pParameter[VCAModulationVolume]->Set (ParameterList[VCAModulationVolume].nDefault);
	}

	return bResult;
}

boolean CPatch::Save (void)
{
	m_Properties.RemoveAll ();

	m_Properties.SetNumber ("Version", 2);

	for (unsigned i = 0; i < PatchPropertyUnknown; i++)
	{
		m_Properties.SetString (PropertyList[i].pName, m_PropertyString[i]);
	}

	for (unsigned i = 0; i < SynthParameterUnknown; i++)
	{
		assert (m_pParameter[i] != 0);
		m_Properties.SetNumber (m_pParameter[i]->GetName (), m_pParameter[i]->Get ());
	}

	return m_Properties.Save ();
}

unsigned CPatch::GetParameter (TSynthParameter Parameter) const
{
	assert (m_pParameter[Parameter] != 0);
	return m_pParameter[Parameter]->Get ();
}

void CPatch::SetParameter (TSynthParameter Parameter, unsigned nValue)
{
	assert (m_pParameter[Parameter] != 0);
	m_pParameter[Parameter]->Set (nValue);
}

void CPatch::SetMIDIParameter (TSynthParameter Parameter, u8 ucValue)
{
	assert (Parameter < SynthParameterUnknown);
	assert (ParameterList[Parameter].Type == ParameterPercent);

	unsigned nValue = (ucValue*100 + 63) / 127;
	nValue =   (nValue + ParameterList[Parameter].nStep/2)
		 / ParameterList[Parameter].nStep
		 * ParameterList[Parameter].nStep;

	if (nValue < ParameterList[Parameter].nMinimum)
	{
		nValue = ParameterList[Parameter].nMinimum;
	}
	else if (nValue > ParameterList[Parameter].nMaximum)
	{
		nValue = ParameterList[Parameter].nMaximum;
	}

	assert (m_pParameter[Parameter] != 0);
	m_pParameter[Parameter]->Set (nValue);
}

boolean CPatch::ParameterDown (TSynthParameter Parameter)
{
	assert (m_pParameter[Parameter] != 0);
	return m_pParameter[Parameter]->Down ();
}

boolean CPatch::ParameterUp (TSynthParameter Parameter)
{
	assert (m_pParameter[Parameter] != 0);
	return m_pParameter[Parameter]->Up ();
}

const char *CPatch::GetParameterHelp (TSynthParameter Parameter)
{
	assert (m_pParameter[Parameter] != 0);
	return m_pParameter[Parameter]->GetHelp ();
}

const char *CPatch::GetParameterString (TSynthParameter Parameter)
{
	assert (m_pParameter[Parameter] != 0);
	return m_pParameter[Parameter]->GetString ();
}

const char *CPatch::GetProperty (TPatchProperty Property) const
{
	assert (Property < PatchPropertyUnknown);
	return m_PropertyString[Property];
}

void CPatch::SetProperty (TPatchProperty Property, const char *pString)
{
	assert (pString != 0);
	assert (Property < PatchPropertyUnknown);
	m_PropertyString[Property] = pString;
}

unsigned CPatch::GetPropertyMaxLength (TPatchProperty Property)
{
	assert (Property < PatchPropertyUnknown);
	return PropertyList[Property].nMaxLength;
}

boolean CPatch::GetPropertyUppercase (TPatchProperty Property)
{
	assert (Property < PatchPropertyUnknown);
	return PropertyList[Property].bUppercase;
}

const char *CPatch::GetPropertyHelp (TPatchProperty Property)
{
	assert (Property < PatchPropertyUnknown);
	return PropertyList[Property].pHelp;
}
