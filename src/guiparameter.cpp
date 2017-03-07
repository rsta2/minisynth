//
// guiparameter.cpp
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
#include "guiparameter.h"
#include <assert.h>

CGUIParameter::CGUIParameter (UG_WINDOW *pWindow, TSynthParameter Parameter, CSynthConfig *pConfig)
:	m_pWindow (pWindow),
	m_Parameter (Parameter),
	m_pConfig (pConfig),
	m_nButtonDownID (-1),
	m_nButtonUpID (-1)
{
	assert (m_pWindow != 0);
	assert (Parameter < SynthParameterUnknown);
	assert (m_pConfig != 0);
}

CGUIParameter::~CGUIParameter (void)
{
	m_pWindow = 0;
	m_pConfig = 0;
}

void CGUIParameter::Create (unsigned nTextboxID, unsigned nButtonDownID, unsigned nButtonUpID,
			    unsigned nPosX, unsigned nPosY)
{
	assert (m_pWindow != 0);

	m_nTextboxID = nTextboxID;
	m_nButtonDownID = nButtonDownID;
	m_nButtonUpID = nButtonUpID;

	UG_ButtonCreate (m_pWindow, &m_ButtonDown, m_nButtonDownID, nPosX, nPosY, nPosX+40, nPosY+25);
	UG_TextboxCreate (m_pWindow, &m_Textbox, m_nTextboxID, nPosX+45, nPosY, nPosX+134, nPosY+25);
	UG_ButtonCreate (m_pWindow, &m_ButtonUp, m_nButtonUpID, nPosX+139, nPosY, nPosX+179, nPosY+25);

	UG_TextboxSetFont (m_pWindow, m_nTextboxID, &FONT_8X14);

	Update ();

	UG_TextboxSetBackColor (m_pWindow, m_nTextboxID, C_LIGHT_GRAY);
	UG_TextboxSetForeColor (m_pWindow, m_nTextboxID, C_BLACK);
	UG_TextboxSetAlignment (m_pWindow, m_nTextboxID, ALIGN_CENTER);

	UG_ButtonSetFont (m_pWindow, m_nButtonDownID, &FONT_8X14);
	UG_ButtonSetText (m_pWindow, m_nButtonDownID, "<");
	UG_ButtonSetFont (m_pWindow, m_nButtonUpID, &FONT_8X14);
	UG_ButtonSetText (m_pWindow, m_nButtonUpID, ">");
}

void CGUIParameter::Update (void)
{
	assert (m_pWindow != 0);
	assert (m_pConfig != 0);

	UG_TextboxSetText (m_pWindow, m_nTextboxID,
			   (char *) m_pConfig->GetActivePatch ()->GetParameterString (m_Parameter));
}

boolean CGUIParameter::ButtonPressed (unsigned nButtonID)
{
	assert (m_pConfig != 0);

	boolean bUpdated = FALSE;
	if (nButtonID == m_nButtonDownID)
	{
		bUpdated = m_pConfig->GetActivePatch ()->ParameterDown (m_Parameter);
	}
	else if (nButtonID == m_nButtonUpID)
	{
		bUpdated = m_pConfig->GetActivePatch ()->ParameterUp (m_Parameter);
	}

	if (bUpdated)
	{
		Update ();
	}

	return bUpdated;
}
