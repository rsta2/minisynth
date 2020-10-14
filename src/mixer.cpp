//
// mixer.cpp
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
#include "mixer.h"
#include <assert.h>

CMixer::CMixer (CSynthModule *pInput1, CSynthModule *pInput2)
:	m_pInput1 (pInput1),
	m_pInput2 (pInput2),
	m_fOutputLevel (0.0)
{
}

CMixer::~CMixer (void)
{
	m_pInput1 = 0;
	m_pInput2 = 0;
}

void CMixer::NextSample (void)
{
	assert (m_pInput1 != 0);
	assert (m_pInput2 != 0);

	m_fOutputLevel = (m_pInput1->GetOutputLevel () + m_pInput2->GetOutputLevel ()) / 2.0;
}

float CMixer::GetOutputLevel (void) const
{
	return m_fOutputLevel;
}
