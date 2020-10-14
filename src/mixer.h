//
// mixer.h
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
#ifndef _mixer_h
#define _mixer_h

#include "synthmodule.h"

class CMixer : public CSynthModule
{
public:
	CMixer (CSynthModule *pInput1, CSynthModule *pInput2);
	~CMixer (void);

	void NextSample (void);
	float GetOutputLevel (void) const;		// returns [-1.0, 1.0]

private:
	CSynthModule *m_pInput1;
	CSynthModule *m_pInput2;

	float m_fOutputLevel;
};

#endif
