//
// notestack.cpp
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
#include "notestack.h"

CNoteStack::CNoteStack (void)
{
}

CNoteStack::~CNoteStack (void)
{
}

void CNoteStack::NoteOn (u8 ucKeyNumber)
{
	void *pNote = (void *) (uintptr) ucKeyNumber;

	m_SpinLock.Acquire ();

	TPtrListElement *pElement = m_Stack.GetFirst ();
	if (pElement == 0)
	{
		m_Stack.InsertAfter (0, pNote);
	}
	else
	{
		m_Stack.InsertBefore (pElement, pNote);
	}

	m_SpinLock.Release ();
}

void CNoteStack::NoteOff (u8 ucKeyNumber)
{
	void *pNote = (void *) (uintptr) ucKeyNumber;

	m_SpinLock.Acquire ();

	TPtrListElement *pElement = m_Stack.Find (pNote);
	if (pElement != 0)
	{
		m_Stack.Remove (pElement);
	}

	m_SpinLock.Release ();
}

u8 CNoteStack::GetCurrentNote (void)
{
	u8 ucResult = NoNote;

	m_SpinLock.Acquire ();

	TPtrListElement *pElement = m_Stack.GetFirst ();
	if (pElement != 0)
	{
		void *pNote = m_Stack.GetPtr (pElement);

		ucResult = (u8) (uintptr) pNote;
	}

	m_SpinLock.Release ();

	return ucResult;
}
