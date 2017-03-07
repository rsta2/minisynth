//
// synthconfig.cpp
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
#include "synthconfig.h"
#include <circle/string.h>
#include <assert.h>

CSynthConfig::CSynthConfig (CFATFileSystem *pFileSystem)
:	m_nActivePatch (0)
{
	assert (pFileSystem != 0);

	for (unsigned i = 0; i < PATCHES; i++)
	{
		CString FileName;
		FileName.Format ("patch%u.txt", i);

		m_pPatch[i] = new CPatch (FileName, pFileSystem);
		assert (m_pPatch[i] != 0);
	}
}

CSynthConfig::~CSynthConfig (void)
{
	for (unsigned i = 0; i < PATCHES; i++)
	{
		delete m_pPatch[i];
		m_pPatch[i] = 0;
	}
}

unsigned CSynthConfig::GetActivePatchNumber (void) const
{
	return m_nActivePatch;
}

void CSynthConfig::SetActivePatchNumber (unsigned nPatch)
{
	assert (nPatch < PATCHES);
	m_nActivePatch = nPatch;
}

CPatch *CSynthConfig::GetActivePatch (void)
{
	assert (m_nActivePatch < PATCHES);
	assert (m_pPatch[m_nActivePatch] != 0);
	return m_pPatch[m_nActivePatch];
}
