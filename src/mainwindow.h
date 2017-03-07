//
// mainwindow.h
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
#ifndef _mainwindow_h
#define _mainwindow_h

#include <ugui/uguicpp.h>
#include "minisynth.h"
#include "synthconfig.h"
#include "guiparameter.h"

class CMainWindow
{
public:
	CMainWindow (CMiniSynthesizer *pSynthesizer, CSynthConfig *pConfig);
	~CMainWindow (void);

private:
	void Callback (UG_MESSAGE *pMsg);
	static void CallbackStub (UG_MESSAGE *pMsg);

	void UpdateAllParameters (void);

private:
	CMiniSynthesizer *m_pSynthesizer;
	CSynthConfig *m_pConfig;

	UG_WINDOW m_Window;

	UG_TEXTBOX m_Textbox1;
	UG_TEXTBOX m_Textbox2;
	UG_TEXTBOX m_Textbox3;
	UG_TEXTBOX m_Textbox4;
	UG_TEXTBOX m_Textbox5;
	UG_TEXTBOX m_Textbox6;
	UG_TEXTBOX m_Textbox7;
	UG_TEXTBOX m_Textbox8;
	UG_TEXTBOX m_Textbox9;

	UG_BUTTON m_Button1;
	UG_BUTTON m_Button2;
	UG_BUTTON m_Button3;
	UG_BUTTON m_Button4;
	UG_BUTTON m_Button5;
	UG_BUTTON m_Button6;
	UG_BUTTON m_Button7;
	UG_BUTTON m_Button8;
	UG_BUTTON m_Button9;
	UG_BUTTON m_Button10;
	UG_BUTTON m_Button11;
	UG_BUTTON m_Button12;

	CGUIParameter m_LFOVCOWaveform;				// contains 3 objects each
	CGUIParameter m_LFOVCOFrequency;
	CGUIParameter m_LFOVCOPulseWidth;
	CGUIParameter m_VCOWaveform;
	CGUIParameter m_VCOPulseWidth;
	CGUIParameter m_VCOModulationFrequency;
	CGUIParameter m_LFOVCAWaveform;
	CGUIParameter m_LFOVCAFrequency;
	CGUIParameter m_LFOVCAPulseWidth;
	CGUIParameter m_EGVCAAttack;
	CGUIParameter m_EGVCADecay;
	CGUIParameter m_EGVCASustain;
	CGUIParameter m_EGVCARelease;
	CGUIParameter m_VCAModulationVolume;
	CGUIParameter m_SynthVolume;

	static const unsigned s_ObjectCount = 9+12+15*3;	// must match the number of objects above
	UG_OBJECT m_ObjectList[s_ObjectCount];

	CPatch *m_pActivePatch;

	UG_COLOR m_ButtonBackColor;

	static CMainWindow *s_pThis;
};

#endif
