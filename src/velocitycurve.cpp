//
// velocitycurve.cpp
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
#include "velocitycurve.h"
#include <circle/string.h>
#include <assert.h>

CVelocityCurve::CVelocityCurve (CFATFileSystem *pFileSystem)
:	m_Properties ("velocity.txt", pFileSystem)
{
	// defaults to linear curve
	for (unsigned i = VELOCITY_MIN; i <= VELOCITY_MAX; i++)
	{
		m_ucVelocity[i] = (u8) i;
	}
}

CVelocityCurve::~CVelocityCurve (void)
{
}

boolean CVelocityCurve::Load (void)
{
	boolean bResult = m_Properties.Load ();
	if (!bResult)
	{
		m_Properties.RemoveAll ();
	}

	for (unsigned i = VELOCITY_MIN; i <= VELOCITY_MAX; i++)
	{
		CString Name;
		Name.Format ("Velocity%u", i);
		unsigned nVelocity = m_Properties.GetNumber (Name, i);

		if (VELOCITY_MIN <= nVelocity && nVelocity <= VELOCITY_MAX)
		{
			m_ucVelocity[i] = (u8) nVelocity;
		}
	}

	return bResult;
}

u8 CVelocityCurve::MapVelocity (u8 ucVelocity) const
{
	assert (VELOCITY_MIN <= ucVelocity && ucVelocity <= VELOCITY_MAX);
	return m_ucVelocity[ucVelocity];
}
