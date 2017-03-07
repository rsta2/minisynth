//
// parameter.h
//
// One parameter of a patch
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
#ifndef _parameter_h
#define _parameter_h

#include <circle/string.h>
#include <circle/types.h>

enum TParameterType
{
	ParameterWaveform,
	ParameterFrequency,
	ParameterTime,
	ParameterPercent,
	ParameterTypeUnknown
};

class CParameter
{
public:
	CParameter (const char *pName, TParameterType Type,
		    unsigned nMinimum, unsigned nMaximum, unsigned nStep, unsigned nDefault);
	~CParameter (void);

	const char *GetName (void);
	unsigned GetDefault (void) const;

	unsigned Get (void) const;
	void Set (unsigned nValue);

	boolean Down (void);			// returns TRUE if value has changed
	boolean Up (void);			// returns TRUE if value has changed

	const char *GetString (void);

private:
	const char *m_pName;
	TParameterType m_Type;
	unsigned m_nMinimum;
	unsigned m_nMaximum;
	unsigned m_nStep;
	unsigned m_nDefault;

	unsigned m_nValue;

	CString m_String;
};

#endif
