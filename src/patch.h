//
// patch.h
//
// Container for patch settings, loadable from properties file
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2017-2022  R. Stange <rsta2@o2online.de>
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
#include <circle/string.h>
#include <circle/types.h>
#include <Properties/propertiesfatfsfile.h>
#include <fatfs/ff.h>

enum TSynthParameter			// the parameters of a patch
{
	// VCO
	LFOVCOWaveform,
	LFOVCOFrequency,

	VCOWaveform,
	VCOModulationVolume,
	VCODetune,

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

	// Effects
	ReverbDecay,
	ReverbVolume,

	// Synth
	SynthVolume,
	MIDIChannel,

	SynthParameterUnknown
};

enum TPatchProperty			// additional string properties
{
	PatchPropertyName,
	PatchPropertyAuthor,
	PatchPropertyComment,

	PatchPropertyUnknown
};

class CPatch
{
public:
	CPatch (const char *pFileName, FATFS *pFileSystem);
	~CPatch (void);

	boolean Load (void);
	boolean Save (void);

	unsigned GetParameter (TSynthParameter Parameter) const;
	void SetParameter (TSynthParameter Parameter, unsigned nValue);

	void SetMIDIParameter (TSynthParameter Parameter, u8 ucValue);

	boolean ParameterDown (TSynthParameter Parameter);	// returns TRUE if value has changed
	boolean ParameterUp (TSynthParameter Parameter);	// returns TRUE if value has changed

	const char *GetParameterHelp (TSynthParameter Parameter);
	const char *GetParameterString (TSynthParameter Parameter);

	boolean IsParameterEditable (TSynthParameter Parameter) const;
	const char *GetParameterEditString (TSynthParameter Parameter);
	void SetParameterEditString (TSynthParameter Parameter, const char *pString);

	const char *GetProperty (TPatchProperty Property) const;
	void SetProperty (TPatchProperty Property, const char *pString);
	static unsigned GetPropertyMaxLength (TPatchProperty Property);
	static boolean GetPropertyUppercase (TPatchProperty Property);
	static const char *GetPropertyHelp (TPatchProperty Property);

	static const char *GetParameterName (TSynthParameter Parameter);

private:
	CPropertiesFatFsFile m_Properties;

	CParameter *m_pParameter[SynthParameterUnknown];

	CString m_PropertyString[PatchPropertyUnknown];
};

#endif
