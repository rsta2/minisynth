//
// mainwindow.cpp
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
#include "mainwindow.h"
#include "config.h"
#include <circle/string.h>
#include <assert.h>

#define LV_COLOR_WHITE		lv_color_white ()
#define LV_COLOR_GRAY		lv_color_make (128, 128, 128)

CMainWindow *CMainWindow::s_pThis = 0;

CMainWindow::CMainWindow (CMiniSynthesizer *pSynthesizer, CSynthConfig *pConfig)
:	m_pSynthesizer (pSynthesizer),
	m_pConfig (pConfig),
	m_pWindow (lv_scr_act ()),
	m_pTabView (lv_tabview_create (m_pWindow)),
	m_pTabMain (lv_tabview_add_tab (m_pTabView, "MAIN")),
	m_pTabPatches (lv_tabview_add_tab (m_pTabView, "PATCHES")),
	m_pLabelStatus (0),
	m_LFOVCOWaveform (m_pTabMain, LFOVCOWaveform, pConfig),
	m_LFOVCOFrequency (m_pTabMain, LFOVCOFrequency, pConfig),
	m_VCOWaveform (m_pTabMain, VCOWaveform, pConfig),
	m_VCODetune (m_pTabMain, VCODetune, pConfig),
	m_VCOModulationVolume (m_pTabMain, VCOModulationVolume, pConfig),
	m_LFOVCFWaveform (m_pTabMain, LFOVCFWaveform, pConfig),
	m_LFOVCFFrequency (m_pTabMain, LFOVCFFrequency, pConfig),
	m_VCFCutoffFrequency (m_pTabMain, VCFCutoffFrequency, pConfig),
	m_VCFResonance (m_pTabMain, VCFResonance, pConfig),
	m_EGVCFAttack (m_pTabMain, EGVCFAttack, pConfig),
	m_EGVCFDecay (m_pTabMain, EGVCFDecay, pConfig),
	m_EGVCFSustain (m_pTabMain, EGVCFSustain, pConfig),
	m_EGVCFRelease (m_pTabMain, EGVCFRelease, pConfig),
	m_VCFModulationVolume (m_pTabMain, VCFModulationVolume, pConfig),
	m_LFOVCAWaveform (m_pTabMain, LFOVCAWaveform, pConfig),
	m_LFOVCAFrequency (m_pTabMain, LFOVCAFrequency, pConfig),
	m_EGVCAAttack (m_pTabMain, EGVCAAttack, pConfig),
	m_EGVCADecay (m_pTabMain, EGVCADecay, pConfig),
	m_EGVCASustain (m_pTabMain, EGVCASustain, pConfig),
	m_EGVCARelease (m_pTabMain, EGVCARelease, pConfig),
	m_VCAModulationVolume (m_pTabMain, VCAModulationVolume, pConfig),
	m_SynthVolume (m_pTabMain, SynthVolume, pConfig),
	m_ReverbDecay (m_pTabMain, ReverbDecay, pConfig),
	m_ReverbVolume (m_pTabMain, ReverbVolume, pConfig),
	m_MIDIChannel (m_pTabMain, MIDIChannel, pConfig),
	m_PropertyName (m_pTabMain, PatchPropertyName, pConfig),
	m_PropertyAuthor (m_pTabMain, PatchPropertyAuthor, pConfig),
	m_PropertyComment (m_pTabMain, PatchPropertyComment, pConfig),
	m_bShowHelp (FALSE)
{
	assert (s_pThis == 0);
	s_pThis = this;

	// setup styles
	lv_style_init (&m_StyleNoBorder);
	lv_style_set_radius (&m_StyleNoBorder, 0);
	lv_style_set_border_width (&m_StyleNoBorder, 0);

	lv_style_init (&m_StyleWhiteBackground);
	lv_style_set_bg_color (&m_StyleWhiteBackground, LV_COLOR_WHITE);

	lv_style_init (&m_StyleGrayBackground);
	lv_style_set_radius (&m_StyleGrayBackground, 0);
	lv_style_set_border_width (&m_StyleGrayBackground, 0);
	lv_style_set_bg_color (&m_StyleGrayBackground, LV_COLOR_GRAY);
	lv_style_set_text_color (&m_StyleGrayBackground, LV_COLOR_WHITE);

	// set window style
	lv_obj_add_style (m_pWindow, &m_StyleWhiteBackground, LV_PART_MAIN);

	// set tabview style
	lv_tabview_set_tab_bar_position (m_pTabView, LV_DIR_TOP);
	lv_tabview_set_tab_bar_size (m_pTabView, ScaleY (50));
	lv_obj_add_style (m_pTabView, &m_StyleWhiteBackground, LV_PART_MAIN);
	lv_obj_set_style_pad_left (lv_tabview_get_tab_btns (m_pTabView), ScaleX (500), 0);

	// create controls
	LabelCreate (m_pWindow, 15, 24, "MiniSynth Pi", LabelStyleTitle);
	// oscillator
	LabelCreate (m_pTabMain, 5, 5, "OSCILLATOR", LabelStyleSection);
	LabelCreate (m_pTabMain, 5, 30, "VCO");
	m_VCOWaveform.Create (10, 60);
	m_VCODetune.Create (10, 90);
	LabelCreate (m_pTabMain, 5, 120, "LFO");
	m_LFOVCOWaveform.Create (10, 150);
	m_LFOVCOFrequency.Create (10, 180);
	m_VCOModulationVolume.Create (10, 210);
	// MIDI
	LabelCreate (m_pTabMain, 5, 270, "MIDI", LabelStyleSection);
	m_MIDIChannel.Create (10, 300);
	// filter
	LabelCreate (m_pTabMain, 205, 5, "FILTER", LabelStyleSection);
	LabelCreate (m_pTabMain, 205, 30, "VCF");
	m_VCFCutoffFrequency.Create (210, 60);
	m_VCFResonance.Create (210, 90);
	LabelCreate (m_pTabMain, 205, 120, "LFO");
	m_LFOVCFWaveform.Create (210, 150);
	m_LFOVCFFrequency.Create (210, 180);
	m_VCFModulationVolume.Create (210, 210);
	LabelCreate (m_pTabMain, 205, 240, "ENVELOPE");
	m_EGVCFAttack.Create (210, 270);
	m_EGVCFDecay.Create (210, 300);
	m_EGVCFSustain.Create (210, 330);
	m_EGVCFRelease.Create (210, 360);
	// amplifier
	LabelCreate (m_pTabMain, 405, 5, "AMPLIFIER", LabelStyleSection);
	LabelCreate (m_pTabMain, 405, 30, "MASTER VOLUME");
	m_SynthVolume.Create (410, 60);
	LabelCreate (m_pTabMain, 405, 120, "LFO");
	m_LFOVCAWaveform.Create (410, 150);
	m_LFOVCAFrequency.Create (410, 180);
	m_VCAModulationVolume.Create (410, 210);
	LabelCreate (m_pTabMain, 405, 240, "ENVELOPE");
	m_EGVCAAttack.Create (410, 270);
	m_EGVCADecay.Create (410, 300);
	m_EGVCASustain.Create (410, 330);
	m_EGVCARelease.Create (410, 360);
	// effects
	LabelCreate (m_pTabMain, 605, 5, "EFFECTS", LabelStyleSection);
	LabelCreate (m_pTabMain, 605, 30, "REVERB");
	m_ReverbDecay.Create (610, 60);
	m_ReverbVolume.Create (610, 90);
	// info
	LabelCreate (m_pTabMain, 605, 150, "INFO", LabelStyleSection);
	m_PropertyName.Create (610, 180);
	m_PropertyAuthor.Create (610, 230);
	m_PropertyComment.Create (610, 280);
	// help
	m_pButtonHelp = ButtonCreate (m_pTabMain, 630, 362, "HELP");
	// patches
	for (unsigned i = 0; i < PATCHES; i++)
	{
		unsigned nPosX = 45 + (i / 13) * 190;
		unsigned nPosY = 10 + (i % 13) * 30;
		m_pButtonPatch[i] = ButtonCreate (m_pTabPatches, nPosX, nPosY, "");
	}
	m_pButtonLoad = ButtonCreate (m_pTabPatches, 615, 310, "LOAD");
	m_pButtonSave = ButtonCreate (m_pTabPatches, 615, 340, "SAVE");

	UpdateAllParameters (TRUE);
}

CMainWindow::~CMainWindow (void)
{
	// TODO: lv_obj_del()

	s_pThis = 0;
}

void CMainWindow::SetHeight (unsigned nPercent)
{
	assert (m_pWindow != 0);
	assert (m_pTabView != 0);

	lv_obj_set_height (m_pTabView, lv_obj_get_height (m_pWindow) * nPercent / 100);
}

void CMainWindow::EventStub (lv_event_t *pEvent)
{
	if (s_pThis != 0)
	{
		s_pThis->EventHandler ((lv_obj_t *) lv_event_get_target (pEvent), lv_event_get_code (pEvent));
	}
}

CMainWindow *CMainWindow::Get (void)
{
	assert (s_pThis != 0);
	return s_pThis;
}

void CMainWindow::EventHandler (lv_obj_t *pObject, lv_event_code_t Event)
{
	assert (m_pSynthesizer != 0);
	assert (m_pConfig != 0);

	if (   Event == LV_EVENT_PRESSED
	    || Event == LV_EVENT_LONG_PRESSED_REPEAT
	    || Event == LV_EVENT_CLICKED)
	{
		if (   // oscillator
		       m_LFOVCOWaveform.EventHandler (pObject, Event, m_bShowHelp)
		    || m_LFOVCOFrequency.EventHandler (pObject, Event, m_bShowHelp)
		    || m_VCOWaveform.EventHandler (pObject, Event, m_bShowHelp)
		    || m_VCOModulationVolume.EventHandler (pObject, Event, m_bShowHelp)
		    || m_VCODetune.EventHandler (pObject, Event, m_bShowHelp)
		       // MIDI
		    || m_MIDIChannel.EventHandler (pObject, Event, m_bShowHelp)
		       // filter
		    || m_VCFCutoffFrequency.EventHandler (pObject, Event, m_bShowHelp)
		    || m_VCFResonance.EventHandler (pObject, Event, m_bShowHelp)
		    || m_LFOVCFWaveform.EventHandler (pObject, Event, m_bShowHelp)
		    || m_LFOVCFFrequency.EventHandler (pObject, Event, m_bShowHelp)
		    || m_VCFModulationVolume.EventHandler (pObject, Event, m_bShowHelp)
		    || m_EGVCFAttack.EventHandler (pObject, Event, m_bShowHelp)
		    || m_EGVCFDecay.EventHandler (pObject, Event, m_bShowHelp)
		    || m_EGVCFSustain.EventHandler (pObject, Event, m_bShowHelp)
		    || m_EGVCFRelease.EventHandler (pObject, Event, m_bShowHelp)
		       // amplifier
		    || m_SynthVolume.EventHandler (pObject, Event, m_bShowHelp)
		    || m_LFOVCAWaveform.EventHandler (pObject, Event, m_bShowHelp)
		    || m_LFOVCAFrequency.EventHandler (pObject, Event, m_bShowHelp)
		    || m_VCAModulationVolume.EventHandler (pObject, Event, m_bShowHelp)
		    || m_EGVCAAttack.EventHandler (pObject, Event, m_bShowHelp)
		    || m_EGVCADecay.EventHandler (pObject, Event, m_bShowHelp)
		    || m_EGVCASustain.EventHandler (pObject, Event, m_bShowHelp)
		    || m_EGVCARelease.EventHandler (pObject, Event, m_bShowHelp)
		       // reverb
		    || m_ReverbDecay.EventHandler (pObject, Event, m_bShowHelp)
		    || m_ReverbVolume.EventHandler (pObject, Event, m_bShowHelp))
		{
			m_pSynthesizer->SetPatch (m_pConfig->GetActivePatch ());

			return;
		}
	}

	if (Event == LV_EVENT_CLICKED)
	{
		for (unsigned i = 0; i < PATCHES; i++)
		{
			if (pObject == m_pButtonPatch[i])
			{
				lv_obj_clear_state (
					m_pButtonPatch[m_pConfig->GetActivePatchNumber ()],
					LV_STATE_CHECKED);
				lv_obj_add_state (m_pButtonPatch[i], LV_STATE_CHECKED);

				m_pConfig->SetActivePatchNumber (i);

				m_pSynthesizer->SetPatch (m_pConfig->GetActivePatch ());
				UpdateAllParameters ();

				return;
			}
		}

		if (pObject == m_pButtonLoad)
		{
			CPatch *pActivePatch = m_pConfig->GetActivePatch ();

			pActivePatch->Load ();
			m_pSynthesizer->SetPatch (pActivePatch);
			UpdateAllParameters (TRUE);

			return;
		}

		if (pObject == m_pButtonSave)
		{
			m_pConfig->GetActivePatch ()->Save ();

			return;
		}

		if (pObject == m_pButtonHelp)
		{
			m_bShowHelp = !m_bShowHelp;
			if (m_bShowHelp)
			{
				lv_obj_add_state (m_pButtonHelp, LV_STATE_CHECKED);
			}
			else
			{
				lv_obj_clear_state (m_pButtonHelp, LV_STATE_CHECKED);
			}
			UpdateAllParameters ();

			return;
		}
	}

	if (   m_PropertyName.EventHandler (pObject, Event)
	    || m_PropertyAuthor.EventHandler (pObject, Event)
	    || m_PropertyComment.EventHandler (pObject, Event))
	{
		return;
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

	// MIDI
	m_MIDIChannel.Update (m_bShowHelp);

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

	// info
	m_PropertyName.Update ();
	m_PropertyAuthor.Update ();
	m_PropertyComment.Update ();

	// patch
	if (bUpdatePatch)
	{
		assert (m_pConfig != 0);
		unsigned nActivePatch = m_pConfig->GetActivePatchNumber ();

		for (unsigned i = 0; i < PATCHES; i++)
		{
			CPatch *pPatch = m_pConfig->GetPatch (i);

			CString Label (pPatch->GetProperty (PatchPropertyName));
			if (Label.GetLength () == 0)
			{
				Label.Format ("%u", i);
			}

			// the only child is the button label
			lv_obj_t *pButtonLabel = lv_obj_get_child (m_pButtonPatch[i], 0);
			assert (pButtonLabel != 0);
			lv_label_set_text (pButtonLabel, Label);

			if (i == nActivePatch)
			{
				lv_obj_add_state (m_pButtonPatch[i], LV_STATE_CHECKED);
			}
			else
			{
				lv_obj_clear_state (m_pButtonPatch[i], LV_STATE_CHECKED);
			}
		}
	}
}

void CMainWindow::UpdateSynthPatch (void)
{
	assert (m_pSynthesizer != 0);
	assert (m_pConfig != 0);

	m_pSynthesizer->SetPatch (m_pConfig->GetActivePatch ());
}

void CMainWindow::UpdateStatus (const char *pString)
{
	assert (pString != 0);

	if (m_pLabelStatus == 0)
	{
		m_pLabelStatus = LabelCreate (m_pWindow, 250, 24, pString, LabelStyleTitle);
	}
	else
	{
		lv_label_set_text (m_pLabelStatus, pString);
	}
}

lv_obj_t *CMainWindow::LabelCreate (lv_obj_t *pParent, unsigned nPosX, unsigned nPosY,
				    const char *pText, TLabelStyle Style)
{
	lv_coord_t nWidth = ScaleX (190);
	lv_coord_t nHeight = ScaleY (24);
	lv_style_t *pStyle = &m_StyleNoBorder;
	switch (Style)
	{
	case LabelStyleTitle:
		nWidth = ScaleX (108);
		break;

	case LabelStyleSubtitle:
		nWidth = ScaleX (250);
		nPosY = ScaleY (nPosY);
		break;

	case LabelStyleSection:
		pStyle = &m_StyleGrayBackground;
		nPosY = ScaleY (nPosY);
		break;

	case LabelStyleDefault:
		nPosY = ScaleY (nPosY);
		break;

	default:
		assert (0);
		break;
	}

	assert (pParent != 0);
	lv_obj_t *pContainer = lv_obj_create (pParent);
	lv_obj_add_style (pContainer, pStyle, LV_PART_MAIN);
	lv_obj_set_size (pContainer, nWidth, nHeight);
	lv_obj_set_pos (pContainer, ScaleX (nPosX), nPosY);
	lv_obj_set_scrollbar_mode (pContainer, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag (pContainer, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t *pLabel = lv_label_create (pContainer);
	assert (pText != 0);
	lv_label_set_text (pLabel, pText);
	lv_obj_set_align (pLabel, LV_ALIGN_CENTER);

	return pLabel;
}

lv_obj_t *CMainWindow::ButtonCreate (lv_obj_t *pParent, unsigned nPosX, unsigned nPosY,
				     const char *pText)
{
	assert (pParent != 0);
	lv_obj_t *pButton = lv_btn_create (pParent);
	lv_obj_set_size (pButton, ScaleX (140), ScaleY (22));
	lv_obj_set_pos (pButton, ScaleX (nPosX), ScaleY (nPosY));
	lv_obj_add_event_cb (pButton, EventStub, LV_EVENT_ALL, 0);

	lv_obj_t *pButtonLabel = lv_label_create (pButton);
	assert (pText != 0);
	lv_label_set_text (pButtonLabel, pText);
	lv_obj_set_align (pButtonLabel, LV_ALIGN_CENTER);

	return pButton;
}

unsigned CMainWindow::ScaleX (unsigned nPos) const
{
	return nPos * lv_obj_get_width (m_pWindow) / 800;
}

unsigned CMainWindow::ScaleY (unsigned nPos) const
{
	return nPos * lv_obj_get_height (m_pWindow) / 480;
}
