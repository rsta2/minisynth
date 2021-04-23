//
// notestack.h
//
// Tracking note events for monophonic mode
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2021  R. Stange <rsta2@o2online.de>
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
#ifndef _notestack_h
#define _notestack_h

#include <circle/ptrlist.h>
#include <circle/spinlock.h>
#include <circle/types.h>

class CNoteStack
{
public:
	static const u8 NoNote = 255;

public:
	CNoteStack (void);
	~CNoteStack (void);

	void NoteOn (u8 ucKeyNumber);
	void NoteOff (u8 ucKeyNumber);

	u8 GetCurrentNote (void);

private:
	CPtrList m_Stack;

	CSpinLock m_SpinLock;
};

#endif
