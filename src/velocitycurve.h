//
// velocitycurve.h
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
#ifndef _velocitycurve_h
#define _velocitycurve_h

#include <circle/fs/fat/fatfs.h>
#include <circle/types.h>
#include <Properties/propertiesfile.h>

#define VELOCITY_MIN	1
#define VELOCITY_MAX	127

class CVelocityCurve
{
public:
	CVelocityCurve (CFATFileSystem *pFileSystem);
	~CVelocityCurve (void);

	boolean Load (void);

	u8 MapVelocity (u8 ucVelocity) const;

private:
	CPropertiesFile m_Properties;

	u8 m_ucVelocity[VELOCITY_MAX+1];
};

#endif
