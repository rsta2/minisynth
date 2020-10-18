//
// guistringproperty.h
//
// GUI representation of one patch string property (e.g. patch name)
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2020  R. Stange <rsta2@o2online.de>
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
#ifndef _guistringproperty_h
#define _guistringproperty_h

#include <lvgl/lvgl.h>
#include "patch.h"
#include "synthconfig.h"
#include <circle/ptrlist.h>
#include <circle/types.h>

class CGUIStringProperty
{
public:
	CGUIStringProperty (lv_obj_t *pWindow, TPatchProperty Property, CSynthConfig *pConfig);
	~CGUIStringProperty (void);

	void Create (unsigned nPosX, unsigned nPosY);

	void Update (void);

	boolean EventHandler (lv_obj_t *pObject, lv_event_t Event);

private:
	static void KeyboardEventHandler (lv_obj_t *pObject, lv_event_t Event);

private:
	lv_obj_t *m_pWindow;
	TPatchProperty m_Property;
	CSynthConfig *m_pConfig;

	lv_obj_t *m_pTextArea;

	static CPtrList s_InstanceList;

	static lv_obj_t *s_pKeyboard;
};

#endif
