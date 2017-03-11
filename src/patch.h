//
// patch.h
//
// Container for patch settings, loadable from properties file
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
#ifndef _patch_h
#define _patch_h

#include "parameter.h"
#include <circle/types.h>
#include <Properties/propertiesfile.h>

enum TSynthParameter			// the parameters of a patch
{
	// VCO
	LFOVCOWaveform,
	LFOVCOFrequency,

	VCOWaveform,
	VCOModulationVolume,

	// VCF
	LFOVCFWaveform,
	LFOVCFFrequency,

	VCFCutoffFrequency,
	VCFResonance,

	EGVCFAttack,
	EGVCFDecay,
	EGVCFSustain,
	EGVCFRelease,

	VCFModulationVolume,

	// VCA
	LFOVCAWaveform,
	LFOVCAFrequency,

	EGVCAAttack,
	EGVCADecay,
	EGVCASustain,
	EGVCARelease,

	VCAModulationVolume,

	// Synth
	SynthVolume,

	SynthParameterUnknown
};

class CPatch
{
public:
	CPatch (const char *pFileName, CFATFileSystem *pFileSystem);
	~CPatch (void);

	boolean Load (void);
	boolean Save (void);

	unsigned GetParameter (TSynthParameter Parameter) const;
	void SetParameter (TSynthParameter Parameter, unsigned nValue);

	boolean ParameterDown (TSynthParameter Parameter);	// returns TRUE if value has changed
	boolean ParameterUp (TSynthParameter Parameter);	// returns TRUE if value has changed

	const char *GetParameterHelp (TSynthParameter Parameter);
	const char *GetParameterString (TSynthParameter Parameter);

private:
	CPropertiesFile m_Properties;

	CParameter *m_pParameter[SynthParameterUnknown];
};

#endif
