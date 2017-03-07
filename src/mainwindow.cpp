//
// mainwindow.cpp
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
#include "mainwindow.h"
#include <assert.h>

enum
{
	TXB_TITLE,

	// headers
	TXB_OSCILLATOR,
	TXB_FILTER,
	TXB_AMPLIFIER,
	TXB_PATCHES,

	// oscillator
	TXB_VCO_WAVEFORM,
	TXB_VCO_PULSE_WIDTH,
	TXB_LFO_VCO,
	TXB_LFO_VCO_WAVEFORM,
	TXB_LFO_VCO_PULSE_WIDTH,
	TXB_LFO_VCO_FREQ,
	TXB_VCO_MODULATION_FREQ,

	// amplifier
	TXB_MASTER_VOLUME,
	TXB_SYNTH_VOLUME,
	TXB_LFO_VCA,
	TXB_LFO_VCA_WAVEFORM,
	TXB_LFO_VCA_PULSE_WIDTH,
	TXB_LFO_VCA_FREQ,
	TXB_VCA_MODULATION_VOLUME,
	TXB_EG_VCA,
	TXB_EG_VCA_ATTACK,
	TXB_EG_VCA_DECAY,
	TXB_EG_VCA_SUSTAIN,
	TXB_EG_VCA_RELEASE
};

enum
{
	// oscillator
	BTN_VCO_WAVEFORM_DOWN,
	BTN_VCO_WAVEFORM_UP,
	BTN_VCO_PULSE_WIDTH_DOWN,
	BTN_VCO_PULSE_WIDTH_UP,
	BTN_LFO_VCO_WAVEFORM_DOWN,
	BTN_LFO_VCO_WAVEFORM_UP,
	BTN_LFO_VCO_PULSE_WIDTH_DOWN,
	BTN_LFO_VCO_PULSE_WIDTH_UP,
	BTN_LFO_VCO_FREQ_DOWN,
	BTN_LFO_VCO_FREQ_UP,
	BTN_VCO_MODULATION_FREQ_DOWN,
	BTN_VCO_MODULATION_FREQ_UP,

	// amplifier
	BTN_SYNTH_VOLUME_DOWN,
	BTN_SYNTH_VOLUME_UP,
	BTN_LFO_VCA_WAVEFORM_DOWN,
	BTN_LFO_VCA_WAVEFORM_UP,
	BTN_LFO_VCA_PULSE_WIDTH_DOWN,
	BTN_LFO_VCA_PULSE_WIDTH_UP,
	BTN_LFO_VCA_FREQ_DOWN,
	BTN_LFO_VCA_FREQ_UP,
	BTN_VCA_MODULATION_VOLUME_DOWN,
	BTN_VCA_MODULATION_VOLUME_UP,
	BTN_EG_VCA_ATTACK_DOWN,
	BTN_EG_VCA_ATTACK_UP,
	BTN_EG_VCA_DECAY_DOWN,
	BTN_EG_VCA_DECAY_UP,
	BTN_EG_VCA_SUSTAIN_DOWN,
	BTN_EG_VCA_SUSTAIN_UP,
	BTN_EG_VCA_RELEASE_DOWN,
	BTN_EG_VCA_RELEASE_UP,

	// patches
	BTN_PATCH_0,
	BTN_PATCH_1,
	BTN_PATCH_2,
	BTN_PATCH_3,
	BTN_PATCH_4,
	BTN_PATCH_5,
	BTN_PATCH_6,
	BTN_PATCH_7,
	BTN_PATCH_8,
	BTN_PATCH_9,
	BTN_LOAD,
	BTN_SAVE
};

CMainWindow *CMainWindow::s_pThis = 0;

CMainWindow::CMainWindow (CMiniSynthesizer *pSynthesizer, CSynthConfig *pConfig)
:	m_pSynthesizer (pSynthesizer),
	m_pConfig (pConfig),
	m_LFOVCOWaveform (&m_Window, LFOVCOWaveform, pConfig),
	m_LFOVCOFrequency (&m_Window, LFOVCOFrequency, pConfig),
	m_LFOVCOPulseWidth (&m_Window, LFOVCOPulseWidth, pConfig),
	m_VCOWaveform (&m_Window, VCOWaveform, pConfig),
	m_VCOPulseWidth (&m_Window, VCOPulseWidth, pConfig),
	m_VCOModulationFrequency (&m_Window, VCOModulationFrequency, pConfig),
	m_LFOVCAWaveform (&m_Window, LFOVCAWaveform, pConfig),
	m_LFOVCAFrequency (&m_Window, LFOVCAFrequency, pConfig),
	m_LFOVCAPulseWidth (&m_Window, LFOVCAPulseWidth, pConfig),
	m_EGVCAAttack (&m_Window, EGVCAAttack, pConfig),
	m_EGVCADecay (&m_Window, EGVCADecay, pConfig),
	m_EGVCASustain (&m_Window, EGVCASustain, pConfig),
	m_EGVCARelease (&m_Window, EGVCARelease, pConfig),
	m_VCAModulationVolume (&m_Window, VCAModulationVolume, pConfig),
	m_SynthVolume (&m_Window, SynthVolume, pConfig),
	m_pActivePatch (m_pConfig->GetActivePatch ())
{
	assert (s_pThis == 0);
	s_pThis = this;

	// create window
	UG_WindowCreate (&m_Window, m_ObjectList, s_ObjectCount, CallbackStub);
	UG_WindowSetStyle (&m_Window, WND_STYLE_2D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize (&m_Window, 0, 0, 799, 479);

	// create controls
	UG_TextboxCreate (&m_Window, &m_Textbox1, TXB_TITLE, 605, 450, 794, 474);
	// headers
	UG_TextboxCreate (&m_Window, &m_Textbox2, TXB_OSCILLATOR, 5, 5, 194, 25);
	UG_TextboxCreate (&m_Window, &m_Textbox3, TXB_FILTER, 205, 5, 394, 25);
	UG_TextboxCreate (&m_Window, &m_Textbox4, TXB_AMPLIFIER, 405, 5, 594, 25);
	UG_TextboxCreate (&m_Window, &m_Textbox5, TXB_PATCHES, 605, 5, 794, 25);
	// oscillator
	m_VCOWaveform.Create (TXB_VCO_WAVEFORM, BTN_VCO_WAVEFORM_DOWN, BTN_VCO_WAVEFORM_UP, 10, 30);
	m_VCOPulseWidth.Create (TXB_VCO_PULSE_WIDTH, BTN_VCO_PULSE_WIDTH_DOWN, BTN_VCO_PULSE_WIDTH_UP, 10, 60);
	UG_TextboxCreate (&m_Window, &m_Textbox6, TXB_LFO_VCO, 5, 120, 194, 145);
	m_LFOVCOWaveform.Create (TXB_LFO_VCO_WAVEFORM, BTN_LFO_VCO_WAVEFORM_DOWN, BTN_LFO_VCO_WAVEFORM_UP, 10, 150);
	m_LFOVCOPulseWidth.Create (TXB_LFO_VCO_PULSE_WIDTH, BTN_LFO_VCO_PULSE_WIDTH_DOWN, BTN_LFO_VCO_PULSE_WIDTH_UP, 10, 180);
	m_LFOVCOFrequency.Create (TXB_LFO_VCO_FREQ, BTN_LFO_VCO_FREQ_DOWN, BTN_LFO_VCO_FREQ_UP, 10, 210);
	m_VCOModulationFrequency.Create (TXB_VCO_MODULATION_FREQ, BTN_VCO_MODULATION_FREQ_DOWN, BTN_VCO_MODULATION_FREQ_UP, 10, 240);
	// amplifier
	UG_TextboxCreate (&m_Window, &m_Textbox7, TXB_MASTER_VOLUME, 405, 30, 594, 55);
	m_SynthVolume.Create (TXB_SYNTH_VOLUME, BTN_SYNTH_VOLUME_DOWN, BTN_SYNTH_VOLUME_UP, 410, 60);
	UG_TextboxCreate (&m_Window, &m_Textbox8, TXB_LFO_VCA, 405, 120, 594, 145);
	m_LFOVCAWaveform.Create (TXB_LFO_VCA_WAVEFORM, BTN_LFO_VCA_WAVEFORM_DOWN, BTN_LFO_VCA_WAVEFORM_UP, 410, 150);
	m_LFOVCAPulseWidth.Create (TXB_LFO_VCA_PULSE_WIDTH, BTN_LFO_VCA_PULSE_WIDTH_DOWN, BTN_LFO_VCA_PULSE_WIDTH_UP, 410, 180);
	m_LFOVCAFrequency.Create (TXB_LFO_VCA_FREQ, BTN_LFO_VCA_FREQ_DOWN, BTN_LFO_VCA_FREQ_UP, 410, 210);
	m_VCAModulationVolume.Create (TXB_VCA_MODULATION_VOLUME, BTN_VCA_MODULATION_VOLUME_DOWN, BTN_VCA_MODULATION_VOLUME_UP, 410, 240);
	UG_TextboxCreate (&m_Window, &m_Textbox9, TXB_EG_VCA, 405, 300, 594, 325);
	m_EGVCAAttack.Create (TXB_EG_VCA_ATTACK, BTN_EG_VCA_ATTACK_DOWN, BTN_EG_VCA_ATTACK_UP, 410, 330);
	m_EGVCADecay.Create (TXB_EG_VCA_DECAY, BTN_EG_VCA_DECAY_DOWN, BTN_EG_VCA_DECAY_UP, 410, 360);
	m_EGVCASustain.Create (TXB_EG_VCA_SUSTAIN, BTN_EG_VCA_SUSTAIN_DOWN, BTN_EG_VCA_SUSTAIN_UP, 410, 390);
	m_EGVCARelease.Create (TXB_EG_VCA_RELEASE, BTN_EG_VCA_RELEASE_DOWN, BTN_EG_VCA_RELEASE_UP, 410, 420);
	// patches
	UG_ButtonCreate (&m_Window, &m_Button1, BTN_PATCH_0, 630, 30, 769, 55);
	UG_ButtonCreate (&m_Window, &m_Button2, BTN_PATCH_1, 630, 60, 769, 85);
	UG_ButtonCreate (&m_Window, &m_Button3, BTN_PATCH_2, 630, 90, 769, 115);
	UG_ButtonCreate (&m_Window, &m_Button4, BTN_PATCH_3, 630, 120, 769, 145);
	UG_ButtonCreate (&m_Window, &m_Button5, BTN_PATCH_4, 630, 150, 769, 175);
	UG_ButtonCreate (&m_Window, &m_Button6, BTN_PATCH_5, 630, 180, 769, 205);
	UG_ButtonCreate (&m_Window, &m_Button7, BTN_PATCH_6, 630, 210, 769, 235);
	UG_ButtonCreate (&m_Window, &m_Button8, BTN_PATCH_7, 630, 240, 769, 265);
	UG_ButtonCreate (&m_Window, &m_Button9, BTN_PATCH_8, 630, 270, 769, 295);
	UG_ButtonCreate (&m_Window, &m_Button10, BTN_PATCH_9, 630, 300, 769, 325);
	UG_ButtonCreate (&m_Window, &m_Button11, BTN_LOAD, 630, 360, 769, 385);
	UG_ButtonCreate (&m_Window, &m_Button12, BTN_SAVE, 630, 390, 769, 415);

	// Title
	UG_TextboxSetFont (&m_Window, TXB_TITLE, &FONT_10X16);
	UG_TextboxSetText (&m_Window, TXB_TITLE, "MiniSynth Pi");
	UG_TextboxSetBackColor (&m_Window, TXB_TITLE, C_LIGHT_GRAY);
	UG_TextboxSetForeColor (&m_Window, TXB_TITLE, C_BLACK);
	UG_TextboxSetAlignment (&m_Window, TXB_TITLE, ALIGN_CENTER);

	// "Oscillator" section
	UG_TextboxSetFont (&m_Window, TXB_OSCILLATOR, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_OSCILLATOR, "OSCILLATOR");
	UG_TextboxSetBackColor (&m_Window, TXB_OSCILLATOR, C_MEDIUM_AQUA_MARINE);
	UG_TextboxSetForeColor (&m_Window, TXB_OSCILLATOR, C_WHITE);
	UG_TextboxSetAlignment (&m_Window, TXB_OSCILLATOR, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_LFO_VCO, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_LFO_VCO, "LFO");
	UG_TextboxSetForeColor (&m_Window, TXB_LFO_VCO, C_BLACK);
	UG_TextboxSetAlignment (&m_Window, TXB_LFO_VCO, ALIGN_CENTER);

	// "Filter" section
	UG_TextboxSetFont (&m_Window, TXB_FILTER, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_FILTER, "FILTER");
	UG_TextboxSetBackColor (&m_Window, TXB_FILTER, C_MEDIUM_AQUA_MARINE);
	UG_TextboxSetForeColor (&m_Window, TXB_FILTER, C_WHITE);
	UG_TextboxSetAlignment (&m_Window, TXB_FILTER, ALIGN_CENTER);

	// "Amplifier" section
	UG_TextboxSetFont (&m_Window, TXB_AMPLIFIER, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_AMPLIFIER, "AMPLIFIER");
	UG_TextboxSetBackColor (&m_Window, TXB_AMPLIFIER, C_MEDIUM_AQUA_MARINE);
	UG_TextboxSetForeColor (&m_Window, TXB_AMPLIFIER, C_WHITE);
	UG_TextboxSetAlignment (&m_Window, TXB_AMPLIFIER, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_MASTER_VOLUME, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_MASTER_VOLUME, "MASTER VOLUME");
	UG_TextboxSetForeColor (&m_Window, TXB_MASTER_VOLUME, C_BLACK);
	UG_TextboxSetAlignment (&m_Window, TXB_MASTER_VOLUME, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_LFO_VCA, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_LFO_VCA, "LFO");
	UG_TextboxSetForeColor (&m_Window, TXB_LFO_VCA, C_BLACK);
	UG_TextboxSetAlignment (&m_Window, TXB_LFO_VCA, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_EG_VCA, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_EG_VCA, "ENVELOPE");
	UG_TextboxSetForeColor (&m_Window, TXB_EG_VCA, C_BLACK);
	UG_TextboxSetAlignment (&m_Window, TXB_EG_VCA, ALIGN_CENTER);

	// "PATCHES" section
	UG_TextboxSetFont (&m_Window, TXB_PATCHES, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_PATCHES, "PATCHES");
	UG_TextboxSetBackColor (&m_Window, TXB_PATCHES, C_MEDIUM_AQUA_MARINE);
	UG_TextboxSetForeColor (&m_Window, TXB_PATCHES, C_WHITE);
	UG_TextboxSetAlignment (&m_Window, TXB_PATCHES, ALIGN_CENTER);

	UG_ButtonSetFont (&m_Window, BTN_PATCH_0, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_0, "0");
	m_ButtonBackColor = UG_ButtonGetBackColor (&m_Window, BTN_PATCH_0);
	UG_ButtonSetBackColor (&m_Window, BTN_PATCH_0, C_MEDIUM_AQUA_MARINE);

	UG_ButtonSetFont (&m_Window, BTN_PATCH_1, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_1, "1");
	UG_ButtonSetFont (&m_Window, BTN_PATCH_2, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_2, "2");
	UG_ButtonSetFont (&m_Window, BTN_PATCH_3, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_3, "3");
	UG_ButtonSetFont (&m_Window, BTN_PATCH_4, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_4, "4");
	UG_ButtonSetFont (&m_Window, BTN_PATCH_5, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_5, "5");
	UG_ButtonSetFont (&m_Window, BTN_PATCH_6, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_6, "6");
	UG_ButtonSetFont (&m_Window, BTN_PATCH_7, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_7, "7");
	UG_ButtonSetFont (&m_Window, BTN_PATCH_8, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_8, "8");
	UG_ButtonSetFont (&m_Window, BTN_PATCH_9, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_9, "9");
	UG_ButtonSetFont (&m_Window, BTN_LOAD, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_LOAD, "LOAD");
	UG_ButtonSetFont (&m_Window, BTN_SAVE, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_SAVE, "SAVE");

	UG_WindowShow (&m_Window);

	UG_Update ();
}

CMainWindow::~CMainWindow (void)
{
	UG_WindowDelete (&m_Window);

	s_pThis = 0;
}

void CMainWindow::Callback (UG_MESSAGE *pMsg)
{
	assert (pMsg != 0);
	if (   pMsg->type  == MSG_TYPE_OBJECT
	    && pMsg->id    == OBJ_TYPE_BUTTON
	    && pMsg->event == OBJ_EVENT_PRESSED)
	{
		assert (m_pSynthesizer != 0);
		assert (m_pConfig != 0);

		unsigned nButtonID = pMsg->sub_id;
		switch (nButtonID)
		{
		// oscillator
		case BTN_VCO_WAVEFORM_DOWN:
		case BTN_VCO_WAVEFORM_UP:
		case BTN_VCO_PULSE_WIDTH_DOWN:
		case BTN_VCO_PULSE_WIDTH_UP:
		case BTN_LFO_VCO_WAVEFORM_DOWN:
		case BTN_LFO_VCO_WAVEFORM_UP:
		case BTN_LFO_VCO_PULSE_WIDTH_DOWN:
		case BTN_LFO_VCO_PULSE_WIDTH_UP:
		case BTN_LFO_VCO_FREQ_DOWN:
		case BTN_LFO_VCO_FREQ_UP:
		case BTN_VCO_MODULATION_FREQ_DOWN:
		case BTN_VCO_MODULATION_FREQ_UP:
		// amplifier
		case BTN_SYNTH_VOLUME_DOWN:
		case BTN_SYNTH_VOLUME_UP:
		case BTN_LFO_VCA_WAVEFORM_DOWN:
		case BTN_LFO_VCA_WAVEFORM_UP:
		case BTN_LFO_VCA_PULSE_WIDTH_DOWN:
		case BTN_LFO_VCA_PULSE_WIDTH_UP:
		case BTN_LFO_VCA_FREQ_DOWN:
		case BTN_LFO_VCA_FREQ_UP:
		case BTN_VCA_MODULATION_VOLUME_DOWN:
		case BTN_VCA_MODULATION_VOLUME_UP:
		case BTN_EG_VCA_ATTACK_DOWN:
		case BTN_EG_VCA_ATTACK_UP:
		case BTN_EG_VCA_DECAY_DOWN:
		case BTN_EG_VCA_DECAY_UP:
		case BTN_EG_VCA_SUSTAIN_DOWN:
		case BTN_EG_VCA_SUSTAIN_UP:
		case BTN_EG_VCA_RELEASE_DOWN:
		case BTN_EG_VCA_RELEASE_UP:
			if (   // oscillator
			       m_LFOVCOWaveform.ButtonPressed (nButtonID)
			    || m_LFOVCOFrequency.ButtonPressed (nButtonID)
			    || m_LFOVCOPulseWidth.ButtonPressed (nButtonID)
			    || m_VCOWaveform.ButtonPressed (nButtonID)
			    || m_VCOPulseWidth.ButtonPressed (nButtonID)
			    || m_VCOModulationFrequency.ButtonPressed (nButtonID)
			       // amplifier
			    || m_SynthVolume.ButtonPressed (nButtonID)
			    || m_LFOVCAWaveform.ButtonPressed (nButtonID)
			    || m_LFOVCAPulseWidth.ButtonPressed (nButtonID)
			    || m_LFOVCAFrequency.ButtonPressed (nButtonID)
			    || m_VCAModulationVolume.ButtonPressed (nButtonID)
			    || m_EGVCAAttack.ButtonPressed (nButtonID)
			    || m_EGVCADecay.ButtonPressed (nButtonID)
			    || m_EGVCASustain.ButtonPressed (nButtonID)
			    || m_EGVCARelease.ButtonPressed (nButtonID))
			{
				m_pSynthesizer->SetPatch (m_pActivePatch);
			}
			break;

		case BTN_PATCH_0:
		case BTN_PATCH_1:
		case BTN_PATCH_2:
		case BTN_PATCH_3:
		case BTN_PATCH_4:
		case BTN_PATCH_5:
		case BTN_PATCH_6:
		case BTN_PATCH_7:
		case BTN_PATCH_8:
		case BTN_PATCH_9:
			UG_ButtonSetBackColor (&m_Window,
					       BTN_PATCH_0+m_pConfig->GetActivePatchNumber (),
					       m_ButtonBackColor);
			UG_ButtonSetBackColor (&m_Window, nButtonID, C_MEDIUM_AQUA_MARINE);

			m_pConfig->SetActivePatchNumber (nButtonID-BTN_PATCH_0);
			m_pActivePatch = m_pConfig->GetActivePatch ();

			m_pSynthesizer->SetPatch (m_pActivePatch);
			UpdateAllParameters ();
			break;

		case BTN_LOAD:
			m_pActivePatch->Load ();
			m_pSynthesizer->SetPatch (m_pActivePatch);
			UpdateAllParameters ();
			break;

		case BTN_SAVE:
			m_pActivePatch->Save ();
			break;

		default:
			break;
		}

	}
}

void CMainWindow::CallbackStub (UG_MESSAGE *pMsg)
{
	assert (s_pThis != 0);
	s_pThis->Callback (pMsg);
}

void CMainWindow::UpdateAllParameters (void)
{
	// oscillator
	m_LFOVCOWaveform.Update ();
	m_LFOVCOFrequency.Update ();
	m_LFOVCOPulseWidth.Update ();
	m_VCOWaveform.Update ();
	m_VCOPulseWidth.Update ();
	m_VCOModulationFrequency.Update ();

	// amplifier
	m_SynthVolume.Update ();
	m_LFOVCAWaveform.Update ();
	m_LFOVCAPulseWidth.Update ();
	m_LFOVCAFrequency.Update ();
	m_VCAModulationVolume.Update ();
	m_EGVCAAttack.Update ();
	m_EGVCADecay.Update ();
	m_EGVCASustain.Update ();
	m_EGVCARelease.Update ();
}
