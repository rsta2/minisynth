//
// mainwindow.cpp
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
#include "mainwindow.h"
#include "config.h"
#include <assert.h>

CMainWindow *CMainWindow::s_pThis = 0;

CMainWindow::CMainWindow (CMiniSynthesizer *pSynthesizer, CSynthConfig *pConfig)
:	m_pSynthesizer (pSynthesizer),
	m_pConfig (pConfig),
	m_pWindow (lv_scr_act ()),
	m_LFOVCOWaveform (m_pWindow, LFOVCOWaveform, pConfig),
	m_LFOVCOFrequency (m_pWindow, LFOVCOFrequency, pConfig),
	m_VCOWaveform (m_pWindow, VCOWaveform, pConfig),
	m_VCODetune (m_pWindow, VCODetune, pConfig),
	m_VCOModulationVolume (m_pWindow, VCOModulationVolume, pConfig),
	m_LFOVCFWaveform (m_pWindow, LFOVCFWaveform, pConfig),
	m_LFOVCFFrequency (m_pWindow, LFOVCFFrequency, pConfig),
	m_VCFCutoffFrequency (m_pWindow, VCFCutoffFrequency, pConfig),
	m_VCFResonance (m_pWindow, VCFResonance, pConfig),
	m_EGVCFAttack (m_pWindow, EGVCFAttack, pConfig),
	m_EGVCFDecay (m_pWindow, EGVCFDecay, pConfig),
	m_EGVCFSustain (m_pWindow, EGVCFSustain, pConfig),
	m_EGVCFRelease (m_pWindow, EGVCFRelease, pConfig),
	m_VCFModulationVolume (m_pWindow, VCFModulationVolume, pConfig),
	m_LFOVCAWaveform (m_pWindow, LFOVCAWaveform, pConfig),
	m_LFOVCAFrequency (m_pWindow, LFOVCAFrequency, pConfig),
	m_EGVCAAttack (m_pWindow, EGVCAAttack, pConfig),
	m_EGVCADecay (m_pWindow, EGVCADecay, pConfig),
	m_EGVCASustain (m_pWindow, EGVCASustain, pConfig),
	m_EGVCARelease (m_pWindow, EGVCARelease, pConfig),
	m_VCAModulationVolume (m_pWindow, VCAModulationVolume, pConfig),
	m_SynthVolume (m_pWindow, SynthVolume, pConfig),
	m_ReverbDecay (m_pWindow, ReverbDecay, pConfig),
	m_ReverbVolume (m_pWindow, ReverbVolume, pConfig),
	m_pActivePatch (m_pConfig->GetActivePatch ()),
	m_bShowHelp (FALSE)
{
	assert (s_pThis == 0);
	s_pThis = this;

	// setup styles
	lv_style_init (&m_StyleNoBorder);
	lv_style_set_radius (&m_StyleNoBorder, LV_STATE_DEFAULT, 0);
	lv_style_set_border_width (&m_StyleNoBorder, LV_STATE_DEFAULT, 0);

	lv_style_init (&m_StyleWhiteBackground);
	lv_style_set_bg_color (&m_StyleWhiteBackground, LV_STATE_DEFAULT, LV_COLOR_WHITE);

	lv_style_init (&m_StyleGrayBackground);
	lv_style_set_radius (&m_StyleGrayBackground, LV_STATE_DEFAULT, 0);
	lv_style_set_border_width (&m_StyleGrayBackground, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color (&m_StyleGrayBackground, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_style_set_text_color (&m_StyleGrayBackground, LV_STATE_DEFAULT, LV_COLOR_WHITE);

	// set window style
	lv_obj_add_style (m_pWindow, LV_OBJ_PART_MAIN, &m_StyleWhiteBackground);

	// create controls
	LabelCreate (5, 450, "MiniSynth Pi", LabelStyleTitle);
	LabelCreate (550, 450, "VIRTUAL ANALOG SYNTHESIZER", LabelStyleSubtitle);
	// oscillator
	LabelCreate (5, 5, "OSCILLATOR", LabelStyleSection);
	LabelCreate (5, 30, "VCO");
	m_VCOWaveform.Create (10, 60);
	m_VCODetune.Create (10, 90);
	LabelCreate (5, 150, "LFO");
	m_LFOVCOWaveform.Create (10, 180);
	m_LFOVCOFrequency.Create (10, 210);
	m_VCOModulationVolume.Create (10, 240);
	// filter
	LabelCreate (205, 5, "FILTER", LabelStyleSection);
	LabelCreate (205, 30, "VCF");
	m_VCFCutoffFrequency.Create (210, 60);
	m_VCFResonance.Create (210, 90);
	LabelCreate (205, 150, "LFO");
	m_LFOVCFWaveform.Create (210, 180);
	m_LFOVCFFrequency.Create (210, 210);
	m_VCFModulationVolume.Create (210, 240);
	LabelCreate (205, 300, "ENVELOPE");
	m_EGVCFAttack.Create (210, 330);
	m_EGVCFDecay.Create (210, 360);
	m_EGVCFSustain.Create (210, 390);
	m_EGVCFRelease.Create (210, 420);
	// amplifier
	LabelCreate (405, 5, "AMPLIFIER", LabelStyleSection);
	LabelCreate (405, 30, "MASTER VOLUME");
	m_SynthVolume.Create (410, 60);
	LabelCreate (405, 150, "LFO");
	m_LFOVCAWaveform.Create (410, 180);
	m_LFOVCAFrequency.Create (410, 210);
	m_VCAModulationVolume.Create (410, 240);
	LabelCreate (405, 300, "ENVELOPE");
	m_EGVCAAttack.Create (410, 330);
	m_EGVCADecay.Create (410, 360);
	m_EGVCASustain.Create (410, 390);
	m_EGVCARelease.Create (410, 420);
	// effects
	LabelCreate (5, 300, "EFFECTS", LabelStyleSection);
	LabelCreate (5, 330, "REVERB");
	m_ReverbDecay.Create (10, 360);
	m_ReverbVolume.Create (10, 390);
	// patches
	LabelCreate (605, 5, "PATCHES", LabelStyleSection);
	for (unsigned i = 0; i < PATCHES; i++)
	{
		char Label[2] = "0";
		Label[0] +=  i;
		m_pButtonPatch[i] = ButtonCreate (630, 32+i*30, Label);
	}
	lv_btn_set_state (m_pButtonPatch[0], LV_BTN_STATE_CHECKED_RELEASED);
	m_pButtonLoad = ButtonCreate (630, 362, "LOAD");
	m_pButtonSave = ButtonCreate (630, 392, "SAVE");
	// help
	m_pButtonHelp = ButtonCreate (30, 422, "HELP");
	UpdateAllParameters ();
}

CMainWindow::~CMainWindow (void)
{
	// TODO: lv_obj_del()

	s_pThis = 0;
}

void CMainWindow::EventStub (lv_obj_t *pObject, lv_event_t Event)
{
	if (s_pThis != 0)
	{
		s_pThis->EventHandler (pObject, Event);
	}
}

void CMainWindow::EventHandler (lv_obj_t *pObject, lv_event_t Event)
{
	assert (m_pSynthesizer != 0);
	assert (m_pConfig != 0);

	if (   Event == LV_EVENT_PRESSED
	    || Event == LV_EVENT_LONG_PRESSED_REPEAT)
	{
		if (   // oscillator
		       m_LFOVCOWaveform.ButtonPressed (pObject, m_bShowHelp)
		    || m_LFOVCOFrequency.ButtonPressed (pObject, m_bShowHelp)
		    || m_VCOWaveform.ButtonPressed (pObject, m_bShowHelp)
		    || m_VCOModulationVolume.ButtonPressed (pObject, m_bShowHelp)
		    || m_VCODetune.ButtonPressed (pObject, m_bShowHelp)
		       // filter
		    || m_VCFCutoffFrequency.ButtonPressed (pObject, m_bShowHelp)
		    || m_VCFResonance.ButtonPressed (pObject, m_bShowHelp)
		    || m_LFOVCFWaveform.ButtonPressed (pObject, m_bShowHelp)
		    || m_LFOVCFFrequency.ButtonPressed (pObject, m_bShowHelp)
		    || m_VCFModulationVolume.ButtonPressed (pObject, m_bShowHelp)
		    || m_EGVCFAttack.ButtonPressed (pObject, m_bShowHelp)
		    || m_EGVCFDecay.ButtonPressed (pObject, m_bShowHelp)
		    || m_EGVCFSustain.ButtonPressed (pObject, m_bShowHelp)
		    || m_EGVCFRelease.ButtonPressed (pObject, m_bShowHelp)
		       // amplifier
		    || m_SynthVolume.ButtonPressed (pObject, m_bShowHelp)
		    || m_LFOVCAWaveform.ButtonPressed (pObject, m_bShowHelp)
		    || m_LFOVCAFrequency.ButtonPressed (pObject, m_bShowHelp)
		    || m_VCAModulationVolume.ButtonPressed (pObject, m_bShowHelp)
		    || m_EGVCAAttack.ButtonPressed (pObject, m_bShowHelp)
		    || m_EGVCADecay.ButtonPressed (pObject, m_bShowHelp)
		    || m_EGVCASustain.ButtonPressed (pObject, m_bShowHelp)
		    || m_EGVCARelease.ButtonPressed (pObject, m_bShowHelp)
		       // reverb
		    || m_ReverbDecay.ButtonPressed (pObject, m_bShowHelp)
		    || m_ReverbVolume.ButtonPressed (pObject, m_bShowHelp))
		{
			m_pSynthesizer->SetPatch (m_pActivePatch);

			return;
		}
	}

	if (Event == LV_EVENT_CLICKED)
	{
		for (unsigned i = 0; i < PATCHES; i++)
		{
			if (pObject == m_pButtonPatch[i])
			{
				lv_btn_set_state (m_pButtonPatch[m_pConfig->GetActivePatchNumber ()],
						  LV_BTN_STATE_RELEASED);
				lv_btn_set_state (m_pButtonPatch[i],
						  LV_BTN_STATE_CHECKED_RELEASED);

				m_pConfig->SetActivePatchNumber (i);
				m_pActivePatch = m_pConfig->GetActivePatch ();

				m_pSynthesizer->SetPatch (m_pActivePatch);
				UpdateAllParameters ();

				return;
			}
		}

		if (pObject == m_pButtonLoad)
		{
			m_pActivePatch->Load ();
			m_pSynthesizer->SetPatch (m_pActivePatch);
			UpdateAllParameters ();

			return;
		}

		if (pObject == m_pButtonSave)
		{
			m_pActivePatch->Save ();

			return;
		}

		if (pObject == m_pButtonHelp)
		{
			m_bShowHelp = !m_bShowHelp;
			lv_btn_set_state (m_pButtonHelp,   m_bShowHelp
							 ? LV_BTN_STATE_CHECKED_RELEASED
							 : LV_BTN_STATE_RELEASED);
			UpdateAllParameters ();

			return;
		}
	}
}

void CMainWindow::UpdateAllParameters (boolean bUpdatePatch)
{
	// oscillator
	m_LFOVCOWaveform.Update (m_bShowHelp);
	m_LFOVCOFrequency.Update (m_bShowHelp);
	m_VCOWaveform.Update (m_bShowHelp);
	m_VCOModulationVolume.Update (m_bShowHelp);
	m_VCODetune.Update (m_bShowHelp);

	// filter
	m_VCFCutoffFrequency.Update (m_bShowHelp);
	m_VCFResonance.Update (m_bShowHelp);
	m_LFOVCFWaveform.Update (m_bShowHelp);
	m_LFOVCFFrequency.Update (m_bShowHelp);
	m_VCFModulationVolume.Update (m_bShowHelp);
	m_EGVCFAttack.Update (m_bShowHelp);
	m_EGVCFDecay.Update (m_bShowHelp);
	m_EGVCFSustain.Update (m_bShowHelp);
	m_EGVCFRelease.Update (m_bShowHelp);

	// amplifier
	m_SynthVolume.Update (m_bShowHelp);
	m_LFOVCAWaveform.Update (m_bShowHelp);
	m_LFOVCAFrequency.Update (m_bShowHelp);
	m_VCAModulationVolume.Update (m_bShowHelp);
	m_EGVCAAttack.Update (m_bShowHelp);
	m_EGVCADecay.Update (m_bShowHelp);
	m_EGVCASustain.Update (m_bShowHelp);
	m_EGVCARelease.Update (m_bShowHelp);

	// reverb
	m_ReverbDecay.Update (m_bShowHelp);
	m_ReverbVolume.Update (m_bShowHelp);

	// patch
	if (bUpdatePatch)
	{
		assert (m_pConfig != 0);
		unsigned nActivePatch = m_pConfig->GetActivePatchNumber ();

		for (unsigned i = 0; i < PATCHES; i++)
		{
			lv_btn_set_state (m_pButtonPatch[i],   i == nActivePatch
							     ? LV_BTN_STATE_CHECKED_RELEASED
							     : LV_BTN_STATE_RELEASED);
		}
	}
}

void CMainWindow::LabelCreate (unsigned nPosX, unsigned nPosY, const char *pText,
			       TLabelStyle Style)
{
	lv_coord_t nWidth = 190;
	lv_style_t *pStyle = &m_StyleNoBorder;
	switch (Style)
	{
	case LabelStyleTitle:
		nWidth = 108;
		break;

	case LabelStyleSubtitle:
		nWidth = 250;
		break;

	case LabelStyleSection:
		pStyle = &m_StyleGrayBackground;
		break;

	case LabelStyleDefault:
		break;

	default:
		assert (0);
		break;
	}

	assert (m_pWindow != 0);
	lv_obj_t *pContainer = lv_cont_create (m_pWindow, 0);
	lv_obj_add_style (pContainer, LV_OBJ_PART_MAIN, pStyle);
	lv_obj_set_size (pContainer, nWidth, 24);
	lv_obj_set_pos (pContainer, nPosX, nPosY);

	lv_obj_t *pLabel = lv_label_create (pContainer, 0);
	assert (pText != 0);
	lv_label_set_text (pLabel, pText);
	lv_label_set_align (pLabel, LV_LABEL_ALIGN_CENTER);
	lv_obj_realign (pLabel);
}

lv_obj_t *CMainWindow::ButtonCreate (unsigned nPosX, unsigned nPosY, const char *pText)
{
	assert (m_pWindow != 0);
	lv_obj_t *pButton = lv_btn_create (m_pWindow, 0);
	lv_obj_set_size (pButton, 140, 22);
	lv_obj_set_pos (pButton, nPosX, nPosY);
	lv_obj_set_event_cb (pButton, EventStub);

	lv_obj_t *pButtonLabel = lv_label_create (pButton, 0);
	assert (pText != 0);
	lv_label_set_text (pButtonLabel, pText);

	return pButton;
}
