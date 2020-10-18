//
// guiparameter.cpp
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
#include "guiparameter.h"
#include "mainwindow.h"
#include "config.h"
#include <assert.h>

CGUIParameter::CGUIParameter (lv_obj_t *pWindow, TSynthParameter Parameter, CSynthConfig *pConfig)
:	m_pWindow (pWindow),
	m_Parameter (Parameter),
	m_pConfig (pConfig),
	m_pContainer (0)
{
	assert (m_pWindow != 0);
	assert (Parameter < SynthParameterUnknown);
	assert (m_pConfig != 0);

	// setup styles
	lv_style_init (&m_StyleNoBorder);
	lv_style_set_radius (&m_StyleNoBorder, LV_STATE_DEFAULT, 0);
	lv_style_set_border_width (&m_StyleNoBorder, LV_STATE_DEFAULT, 0);

	lv_style_init (&m_StyleSilverBackground);
	lv_style_set_radius (&m_StyleSilverBackground, LV_STATE_DEFAULT, 0);
	lv_style_set_border_width (&m_StyleSilverBackground, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color (&m_StyleSilverBackground, LV_STATE_DEFAULT, LV_COLOR_SILVER);
}

CGUIParameter::~CGUIParameter (void)
{
	if (m_pContainer != 0)
	{
		lv_obj_del (m_pContainer);
	}

	m_pWindow = 0;
	m_pConfig = 0;
}

void CGUIParameter::Create (unsigned nPosX, unsigned nPosY)
{
	assert (m_pWindow != 0);

	m_pContainer = lv_cont_create (m_pWindow, 0);
	lv_obj_add_style (m_pContainer, LV_OBJ_PART_MAIN, &m_StyleNoBorder);
	lv_obj_set_size (m_pContainer, 180, 26);
	lv_obj_set_pos (m_pContainer, nPosX, nPosY);

	m_pButtonDown = lv_btn_create (m_pContainer, 0);
	lv_obj_set_size (m_pButtonDown, 38, 22);
	lv_obj_set_pos (m_pButtonDown, 2, 2);
	lv_obj_set_event_cb (m_pButtonDown, CMainWindow::EventStub);
	m_pButtonDownLabel = lv_label_create (m_pButtonDown, 0);
	lv_label_set_text (m_pButtonDownLabel, "<");

	m_pContainerText = lv_cont_create (m_pContainer, 0);
	lv_obj_add_style (m_pContainerText, LV_OBJ_PART_MAIN, &m_StyleSilverBackground);
	lv_obj_set_size (m_pContainerText, 86, 22);
	lv_obj_set_pos (m_pContainerText, 47, 2);

	m_pTextLabel = lv_label_create (m_pContainerText, 0);
	lv_label_set_align (m_pTextLabel, LV_LABEL_ALIGN_CENTER);

	m_pButtonUp = lv_btn_create (m_pContainer, 0);
	lv_obj_set_size (m_pButtonUp, 38, 22);
	lv_obj_set_pos (m_pButtonUp, 140, 2);
	lv_obj_set_event_cb (m_pButtonUp, CMainWindow::EventStub);
	m_pButtonUpLabel = lv_label_create (m_pButtonUp, 0);
	lv_label_set_text (m_pButtonUpLabel, ">");

	Update (FALSE);
}

void CGUIParameter::Update (boolean bShowHelp)
{
	assert (m_pTextLabel != 0);
	assert (m_pConfig != 0);

	const char *pText;
	if (bShowHelp)
	{
		pText = m_pConfig->GetActivePatch ()->GetParameterHelp (m_Parameter);
	}
	else
	{
		pText = m_pConfig->GetActivePatch ()->GetParameterString (m_Parameter);
	}

	lv_label_set_text (m_pTextLabel, pText);
	lv_obj_realign (m_pTextLabel);
}

boolean CGUIParameter::ButtonPressed (lv_obj_t *pObject, boolean bShowHelp)
{
	assert (m_pConfig != 0);

	boolean bUpdated = FALSE;
	if (pObject == m_pButtonDown)
	{
		bUpdated = m_pConfig->GetActivePatch ()->ParameterDown (m_Parameter);
	}
	else if (pObject == m_pButtonUp)
	{
		bUpdated = m_pConfig->GetActivePatch ()->ParameterUp (m_Parameter);
	}

	if (bUpdated)
	{
		Update (bShowHelp);
	}

	return bUpdated;
}
