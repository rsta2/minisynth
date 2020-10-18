//
// synthconfig.h
//
// Container for global configuration and all patches
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
#ifndef _synthconfig_h
#define _synthconfig_h

#include <fatfs/ff.h>
#include "patch.h"
#include "velocitycurve.h"
#include "config.h"

class CSynthConfig
{
public:
	CSynthConfig (FATFS *pFileSystem);
	~CSynthConfig (void);

	// loads global configuration only, not the patches
	boolean Load (void);

	// the patch which is currently active
	unsigned GetActivePatchNumber (void) const;
	void SetActivePatchNumber (unsigned nPatch);

	// get the active patch
	CPatch *GetActivePatch (void);

	// get patch by number
	CPatch *GetPatch (unsigned nPatch);

	u8 MapVelocity (u8 ucVelocity) const;

private:
	FATFS *m_pFileSystem;

	CPatch *m_pPatch[PATCHES];
	unsigned m_nActivePatch;

	CVelocityCurve m_VelocityCurve;
};

#endif
