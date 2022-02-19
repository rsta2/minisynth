//
// guiparameter.h
//
// GUI representation of one patch parameter
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2017-2022  R. Stange <rsta2@o2online.de>
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
#ifndef _guiparameter_h
#define _guiparameter_h

#include <lvgl/lvgl.h>
#include "patch.h"
#include "synthconfig.h"
#include <circle/types.h>

class CGUIParameter
{
public:
	CGUIParameter (lv_obj_t *pWindow, TSynthParameter Parameter, CSynthConfig *pConfig);
	~CGUIParameter (void);

	void Create (unsigned nPosX, unsigned nPosY);

	void Update (boolean bShowHelp);

	boolean EventHandler (lv_obj_t *pObject, lv_event_code_t Event, boolean bShowHelp);

private:
	boolean ButtonPressed (lv_obj_t *pObject, boolean bShowHelp);

	void KeyboardEventHandler (lv_obj_t *pObject, lv_event_code_t Event);
	static void KeyboardEventStub (lv_event_t *pEvent);

	unsigned ScaleX (unsigned nPos) const;
	unsigned ScaleY (unsigned nPos) const;

private:
	lv_obj_t *m_pWindow;
	TSynthParameter m_Parameter;
	CSynthConfig *m_pConfig;

	lv_style_t m_StyleNoBorder;
	lv_style_t m_StyleSilverBackground;

	unsigned m_nPosX;
	unsigned m_nPosY;

	lv_obj_t *m_pContainer;
	lv_obj_t *m_pButtonDown;
	lv_obj_t *m_pButtonDownLabel;
	lv_obj_t *m_pContainerText;
	lv_obj_t *m_pTextLabel;
	lv_obj_t *m_pButtonUp;
	lv_obj_t *m_pButtonUpLabel;

	lv_obj_t *m_pTextArea;
	lv_obj_t *m_pKeyboard;

	static CGUIParameter *s_pCurrentEdit;
};

#endif
