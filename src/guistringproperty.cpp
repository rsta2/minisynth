//
// guistringproperty.cpp
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2020-2024  R. Stange <rsta2@o2online.de>
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
#include "guistringproperty.h"
#include "mainwindow.h"
#include <assert.h>

struct TInstanceInfo			// map pointer to text area -> instance of this class
{
	lv_obj_t *pTextArea;
	CGUIStringProperty *pThis;
};

CPtrList CGUIStringProperty::s_InstanceList;

lv_obj_t *CGUIStringProperty::s_pKeyboard = 0;

CGUIStringProperty::CGUIStringProperty (lv_obj_t *pWindow, TPatchProperty Property,
					CSynthConfig *pConfig)
:	m_pWindow (pWindow),
	m_Property (Property),
	m_pConfig (pConfig),
	m_pTextArea (0)
{
}

CGUIStringProperty::~CGUIStringProperty (void)
{
	if (s_pKeyboard != 0)
	{
		lv_obj_del (s_pKeyboard);
		s_pKeyboard = 0;
	}

	if (m_pTextArea != 0)
	{
		lv_obj_del (m_pTextArea);
		m_pTextArea = 0;
	}

	// TODO: clear s_InstanceList

	m_pWindow = 0;
	m_pConfig = 0;
}

void CGUIStringProperty::Create (unsigned nPosX, unsigned nPosY)
{
	assert (m_pWindow != 0);
	assert (m_pConfig != 0);

	m_pTextArea = lv_textarea_create (m_pWindow);
	assert (m_pTextArea != 0);
	lv_obj_set_size (m_pTextArea, ScaleX (180), ScaleY (25));  // TODO: height does not apply
	lv_obj_set_pos (m_pTextArea, ScaleX (nPosX), ScaleY (nPosY));

	lv_textarea_set_max_length (m_pTextArea, CPatch::GetPropertyMaxLength (m_Property));
	lv_textarea_set_placeholder_text (m_pTextArea, CPatch::GetPropertyHelp (m_Property));
	lv_textarea_set_text (m_pTextArea, m_pConfig->GetActivePatch ()->GetProperty (m_Property));
	lv_textarea_set_one_line (m_pTextArea, TRUE);
	lv_obj_add_event_cb (m_pTextArea, CMainWindow::EventStub, LV_EVENT_ALL, 0);

	TInstanceInfo *pInfo = new TInstanceInfo;
	assert (pInfo != 0);
	pInfo->pTextArea = m_pTextArea;
	pInfo->pThis = this;
	s_InstanceList.InsertAfter (s_InstanceList.GetFirst (), pInfo);
}

void CGUIStringProperty::Update (void)
{
	assert (m_pTextArea != 0);
	assert (m_pConfig != 0);

	lv_textarea_set_text (m_pTextArea, m_pConfig->GetActivePatch ()->GetProperty (m_Property));
}

boolean CGUIStringProperty::EventHandler (lv_obj_t *pObject, lv_event_code_t Event)
{
	if (pObject != m_pTextArea)
	{
		return FALSE;
	}

	assert (m_pWindow != 0);

	if (Event == LV_EVENT_RELEASED)
	{
		if (s_pKeyboard == 0)
		{
			CMainWindow::Get ()->SetHeight (50);

			s_pKeyboard = lv_keyboard_create (lv_scr_act ());
			assert (s_pKeyboard != 0);

			lv_keyboard_set_mode (s_pKeyboard,
					        CPatch::GetPropertyUppercase (m_Property)
					      ? LV_KEYBOARD_MODE_TEXT_UPPER
					      : LV_KEYBOARD_MODE_TEXT_LOWER);
			lv_obj_add_event_cb (s_pKeyboard, KeyboardEventHandler, LV_EVENT_ALL, 0);

			lv_indev_wait_release (lv_indev_get_act ());
		}

		lv_obj_add_state (m_pTextArea, LV_STATE_FOCUSED);
		lv_obj_scroll_to_view (m_pTextArea, LV_ANIM_OFF);
		lv_keyboard_set_textarea (s_pKeyboard, m_pTextArea);
	}
	else if (Event == LV_EVENT_DEFOCUSED)
	{
		if (s_pKeyboard != 0)
		{
			CMainWindow::Get ()->SetHeight (100);
			lv_obj_del (s_pKeyboard);
			s_pKeyboard = 0;
		}
	}

	return TRUE;
}

void CGUIStringProperty::KeyboardEventHandler (lv_event_t *pEvent)
{
	lv_obj_t *pObject = (lv_obj_t *) lv_event_get_target (pEvent);
	lv_event_code_t Event = lv_event_get_code (pEvent);

	if (pObject != s_pKeyboard)
	{
		return;
	}

	if (Event == LV_EVENT_CANCEL)
	{
		CMainWindow::Get ()->UpdateAllParameters ();

		lv_obj_t *pTextArea = lv_keyboard_get_textarea (s_pKeyboard);
		assert (pTextArea != 0);
		lv_obj_clear_state (pTextArea, LV_STATE_FOCUSED);

		CMainWindow::Get ()->SetHeight (100);
		lv_obj_del (s_pKeyboard);
		s_pKeyboard = 0;
	}
	else if (Event == LV_EVENT_READY)
	{
		lv_obj_t *pTextArea = lv_keyboard_get_textarea (s_pKeyboard);
		assert (pTextArea != 0);

		lv_obj_clear_state (pTextArea, LV_STATE_FOCUSED);

		// get this pointer for pTextArea
		CGUIStringProperty *pThis = 0;
		TPtrListElement *pElement = s_InstanceList.GetFirst ();
		while (pElement != 0)
		{
			TInstanceInfo *pInfo = (TInstanceInfo *) s_InstanceList.GetPtr (pElement);
			if (pInfo->pTextArea == pTextArea)
			{
				pThis = pInfo->pThis;

				break;
			}

			pElement = s_InstanceList.GetNext (pElement);
		}

		assert (pThis != 0);
		assert (pThis->m_pConfig != 0);
		pThis->m_pConfig->GetActivePatch ()->SetProperty (pThis->m_Property,
								  lv_textarea_get_text (pTextArea));

		CMainWindow::Get ()->UpdateAllParameters (TRUE);

		CMainWindow::Get ()->SetHeight (100);
		lv_obj_del (s_pKeyboard);
		s_pKeyboard = 0;
	}
}

unsigned CGUIStringProperty::ScaleX (unsigned nPos) const
{
	return nPos * lv_obj_get_width (lv_scr_act ()) / 800;
}

unsigned CGUIStringProperty::ScaleY (unsigned nPos) const
{
	return nPos * lv_obj_get_height (lv_scr_act ()) / 480;
}
