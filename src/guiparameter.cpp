//
// guiparameter.cpp
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2017-2024  R. Stange <rsta2@o2online.de>
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

#define LV_COLOR_SILVER		lv_color_make (192, 192, 192)

CGUIParameter *CGUIParameter::s_pCurrentEdit = 0;

CGUIParameter::CGUIParameter (lv_obj_t *pWindow, TSynthParameter Parameter, CSynthConfig *pConfig)
:	m_pWindow (pWindow),
	m_Parameter (Parameter),
	m_pConfig (pConfig),
	m_pContainer (0),
	m_pTextArea (0),
	m_pKeyboard (0)
{
	assert (m_pWindow != 0);
	assert (Parameter < SynthParameterUnknown);
	assert (m_pConfig != 0);

	// setup styles
	lv_style_init (&m_StyleNoBorder);
	lv_style_set_radius (&m_StyleNoBorder, 0);
	lv_style_set_border_width (&m_StyleNoBorder, 0);

	lv_style_init (&m_StyleSilverBackground);
	lv_style_set_radius (&m_StyleSilverBackground, 0);
	lv_style_set_border_width (&m_StyleSilverBackground, 0);
	lv_style_set_bg_color (&m_StyleSilverBackground, LV_COLOR_SILVER);
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

	m_nPosX = nPosX;
	m_nPosY = nPosY;

	m_pContainer = lv_obj_create (m_pWindow);
	lv_obj_add_style (m_pContainer, &m_StyleNoBorder, LV_PART_MAIN);
	lv_obj_set_size (m_pContainer, ScaleX (180), ScaleY (26));
	lv_obj_set_pos (m_pContainer, ScaleX (nPosX), ScaleY (nPosY));
	lv_obj_set_style_pad_top (m_pContainer, 0, 0);
	lv_obj_set_style_pad_left (m_pContainer, 0, 0);
	lv_obj_set_scrollbar_mode (m_pContainer, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag (m_pContainer, LV_OBJ_FLAG_SCROLLABLE);

	m_pButtonDown = lv_btn_create (m_pContainer);
	lv_obj_set_size (m_pButtonDown, ScaleX (38), ScaleY (22));
	lv_obj_set_pos (m_pButtonDown, ScaleX (2), ScaleY (2));
	lv_obj_add_event_cb (m_pButtonDown, CMainWindow::EventStub, LV_EVENT_ALL, 0);
	m_pButtonDownLabel = lv_label_create (m_pButtonDown);
	lv_label_set_text (m_pButtonDownLabel, "<");
	lv_obj_set_align (m_pButtonDownLabel, LV_ALIGN_CENTER);

	m_pContainerText = lv_obj_create (m_pContainer);
	lv_obj_add_style (m_pContainerText, &m_StyleSilverBackground, LV_PART_MAIN);
	lv_obj_set_size (m_pContainerText, ScaleX (86), ScaleY (22));
	lv_obj_set_pos (m_pContainerText, ScaleX (47), ScaleY (2));
	lv_obj_set_scrollbar_mode (m_pContainerText, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag (m_pContainerText, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_event_cb (m_pContainerText, CMainWindow::EventStub, LV_EVENT_ALL, 0);

	m_pTextLabel = lv_label_create (m_pContainerText);
	lv_obj_set_align (m_pTextLabel, LV_ALIGN_CENTER);

	m_pButtonUp = lv_btn_create (m_pContainer);
	lv_obj_set_size (m_pButtonUp, ScaleX (38), ScaleY (22));
	lv_obj_set_pos (m_pButtonUp, ScaleX (140), ScaleY (2));
	lv_obj_add_event_cb (m_pButtonUp, CMainWindow::EventStub, LV_EVENT_ALL, 0);
	m_pButtonUpLabel = lv_label_create (m_pButtonUp);
	lv_label_set_text (m_pButtonUpLabel, ">");
	lv_obj_set_align (m_pButtonUpLabel, LV_ALIGN_CENTER);

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

boolean CGUIParameter::EventHandler (lv_obj_t *pObject, lv_event_code_t Event, boolean bShowHelp)
{
	assert (m_pConfig != 0);

	if (   Event == LV_EVENT_PRESSED
	    || Event == LV_EVENT_LONG_PRESSED_REPEAT)
	{
		return ButtonPressed (pObject, bShowHelp);
	}

	if (   pObject != m_pContainerText
	    || Event != LV_EVENT_CLICKED
	    || s_pCurrentEdit != 0
	    || !m_pConfig->GetActivePatch ()->IsParameterEditable (m_Parameter))
	{
		return FALSE;
	}

	assert (s_pCurrentEdit == 0);
	s_pCurrentEdit = this;

	// create text area
	assert (m_pTextArea == 0);
	m_pTextArea = lv_textarea_create (m_pWindow);
	assert (m_pTextArea != 0);
	lv_obj_set_size (m_pTextArea, ScaleX (180), ScaleY (26));  // TODO: height does not apply
	lv_obj_set_pos (m_pTextArea, ScaleX (m_nPosX), ScaleY (m_nPosY));

	lv_textarea_set_max_length (m_pTextArea, 10);
	lv_textarea_set_text (m_pTextArea,
			      m_pConfig->GetActivePatch ()->GetParameterEditString (m_Parameter));
	lv_textarea_set_one_line (m_pTextArea, TRUE);

	// open screen keyboard
	CMainWindow::Get ()->SetHeight (50);

	assert (m_pKeyboard == 0);
	m_pKeyboard = lv_keyboard_create (lv_scr_act ());
	assert (m_pKeyboard != 0);

	lv_keyboard_set_mode (m_pKeyboard, LV_KEYBOARD_MODE_NUMBER);
	lv_obj_add_event_cb (m_pKeyboard, KeyboardEventStub, LV_EVENT_ALL, 0);

	lv_indev_wait_release (lv_indev_get_act ());

	lv_obj_add_state (m_pTextArea, LV_STATE_FOCUSED);
	lv_obj_scroll_to_view (m_pTextArea, LV_ANIM_OFF);
	lv_keyboard_set_textarea (m_pKeyboard, m_pTextArea);

	return FALSE;
}

void CGUIParameter::KeyboardEventHandler (lv_obj_t *pObject, lv_event_code_t Event)
{
	assert (pObject == m_pKeyboard);

	if (Event == LV_EVENT_CANCEL)
	{
		CMainWindow::Get ()->SetHeight (100);

		lv_obj_del (m_pKeyboard);
		m_pKeyboard = 0;

		lv_obj_del (m_pTextArea);
		m_pTextArea = 0;

		s_pCurrentEdit = 0;
	}
	else if (Event == LV_EVENT_READY)
	{
		assert (m_pConfig != 0);
		m_pConfig->GetActivePatch ()->SetParameterEditString (m_Parameter,
								lv_textarea_get_text (m_pTextArea));

		CMainWindow::Get ()->UpdateAllParameters (FALSE);
		CMainWindow::Get ()->UpdateSynthPatch ();

		CMainWindow::Get ()->SetHeight (100);

		lv_obj_del (m_pKeyboard);
		m_pKeyboard = 0;

		lv_obj_del (m_pTextArea);
		m_pTextArea = 0;

		s_pCurrentEdit = 0;
	}
}

void CGUIParameter::KeyboardEventStub (lv_event_t *pEvent)
{
	assert (s_pCurrentEdit != 0);
	s_pCurrentEdit->KeyboardEventHandler ((lv_obj_t *) lv_event_get_target (pEvent),
					      lv_event_get_code (pEvent));
}

unsigned CGUIParameter::ScaleX (unsigned nPos) const
{
	return nPos * lv_obj_get_width (lv_scr_act ()) / 800;
}

unsigned CGUIParameter::ScaleY (unsigned nPos) const
{
	return nPos * lv_obj_get_height (lv_scr_act ()) / 480;
}
