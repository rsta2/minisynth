//
// mainwindow.h
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
#ifndef _mainwindow_h
#define _mainwindow_h

#include <lvgl/lvgl.h>
#include "minisynth.h"
#include "synthconfig.h"
#include "guiparameter.h"
#include "guistringproperty.h"
#include <circle/types.h>

class CMainWindow
{
public:
	CMainWindow (CMiniSynthesizer *pSynthesizer, CSynthConfig *pConfig);
	~CMainWindow (void);

	void SetHeight (unsigned nPercent);

	void UpdateAllParameters (boolean bUpdatePatch = FALSE);

	void UpdateSynthPatch (void);

	void UpdateStatus (const char *pString);

	static void EventStub (lv_event_t *pEvent);

	static CMainWindow *Get (void);

private:
	void EventHandler (lv_obj_t *pObject, lv_event_code_t Event);

	enum TLabelStyle
	{
		LabelStyleTitle,
		LabelStyleSubtitle,
		LabelStyleSection,
		LabelStyleDefault
	};
	lv_obj_t *LabelCreate (lv_obj_t *pParent, unsigned nPosX, unsigned nPosY, const char *pText,
			       TLabelStyle Style = LabelStyleDefault);

	lv_obj_t *ButtonCreate (lv_obj_t *pParent, unsigned nPosX, unsigned nPosY,
				const char *pText);

	unsigned ScaleX (unsigned nPos) const;
	unsigned ScaleY (unsigned nPos) const;

private:
	CMiniSynthesizer *m_pSynthesizer;
	CSynthConfig *m_pConfig;

	lv_style_t m_StyleNoBorder;
	lv_style_t m_StyleWhiteBackground;
	lv_style_t m_StyleGrayBackground;

	lv_obj_t *m_pWindow;

	lv_obj_t *m_pTabView;
	lv_obj_t *m_pTabMain;
	lv_obj_t *m_pTabPatches;

	lv_obj_t *m_pButtonPatch[PATCHES];
	lv_obj_t *m_pButtonLoad;
	lv_obj_t *m_pButtonSave;
	lv_obj_t *m_pButtonHelp;

	lv_obj_t *m_pLabelStatus;

	CGUIParameter m_LFOVCOWaveform;
	CGUIParameter m_LFOVCOFrequency;
	CGUIParameter m_VCOWaveform;
	CGUIParameter m_VCODetune;
	CGUIParameter m_VCOModulationVolume;
	CGUIParameter m_LFOVCFWaveform;
	CGUIParameter m_LFOVCFFrequency;
	CGUIParameter m_VCFCutoffFrequency;
	CGUIParameter m_VCFResonance;
	CGUIParameter m_EGVCFAttack;
	CGUIParameter m_EGVCFDecay;
	CGUIParameter m_EGVCFSustain;
	CGUIParameter m_EGVCFRelease;
	CGUIParameter m_VCFModulationVolume;
	CGUIParameter m_LFOVCAWaveform;
	CGUIParameter m_LFOVCAFrequency;
	CGUIParameter m_EGVCAAttack;
	CGUIParameter m_EGVCADecay;
	CGUIParameter m_EGVCASustain;
	CGUIParameter m_EGVCARelease;
	CGUIParameter m_VCAModulationVolume;
	CGUIParameter m_SynthVolume;
	CGUIParameter m_ReverbDecay;
	CGUIParameter m_ReverbVolume;
	CGUIParameter m_MIDIChannel;

	CGUIStringProperty m_PropertyName;
	CGUIStringProperty m_PropertyAuthor;
	CGUIStringProperty m_PropertyComment;

	boolean m_bShowHelp;

	static CMainWindow *s_pThis;
};

#endif
