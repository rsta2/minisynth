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

enum
{
	TXB_TITLE,
	TXB_SUBTITLE,

	// headers
	TXB_OSCILLATOR,
	TXB_FILTER,
	TXB_AMPLIFIER,
	TXB_EFFECTS,
	TXB_PATCHES,

	// oscillator
	TXB_VCO,
	TXB_VCO_WAVEFORM,
	TXB_LFO_VCO,
	TXB_LFO_VCO_WAVEFORM,
	TXB_LFO_VCO_FREQ,
	TXB_VCO_MODULATION_VOLUME,

	// filter
	TXB_VCF,
	TXB_VCF_CUTOFF_FREQ,
	TXB_VCF_RESONANCE,
	TXB_LFO_VCF,
	TXB_LFO_VCF_WAVEFORM,
	TXB_LFO_VCF_FREQ,
	TXB_VCF_MODULATION_VOLUME,
	TXB_EG_VCF,
	TXB_EG_VCF_ATTACK,
	TXB_EG_VCF_DECAY,
	TXB_EG_VCF_SUSTAIN,
	TXB_EG_VCF_RELEASE,

	// amplifier
	TXB_MASTER_VOLUME,
	TXB_SYNTH_VOLUME,
	TXB_LFO_VCA,
	TXB_LFO_VCA_WAVEFORM,
	TXB_LFO_VCA_FREQ,
	TXB_VCA_MODULATION_VOLUME,
	TXB_EG_VCA,
	TXB_EG_VCA_ATTACK,
	TXB_EG_VCA_DECAY,
	TXB_EG_VCA_SUSTAIN,
	TXB_EG_VCA_RELEASE,

	// reverb
	TXB_REVERB,
	TXB_REVERB_DECAY,
	TXB_REVERB_VOLUME
};

enum
{
	// oscillator
	BTN_VCO_WAVEFORM_DOWN,
	BTN_VCO_WAVEFORM_UP,
	BTN_LFO_VCO_WAVEFORM_DOWN,
	BTN_LFO_VCO_WAVEFORM_UP,
	BTN_LFO_VCO_FREQ_DOWN,
	BTN_LFO_VCO_FREQ_UP,
	BTN_VCO_MODULATION_VOLUME_DOWN,
	BTN_VCO_MODULATION_VOLUME_UP,

	// filter
	BTN_VCF_CUTOFF_FREQ_DOWN,
	BTN_VCF_CUTOFF_FREQ_UP,
	BTN_VCF_RESONANCE_DOWN,
	BTN_VCF_RESONANCE_UP,
	BTN_LFO_VCF_WAVEFORM_DOWN,
	BTN_LFO_VCF_WAVEFORM_UP,
	BTN_LFO_VCF_FREQ_DOWN,
	BTN_LFO_VCF_FREQ_UP,
	BTN_VCF_MODULATION_VOLUME_DOWN,
	BTN_VCF_MODULATION_VOLUME_UP,
	BTN_EG_VCF_ATTACK_DOWN,
	BTN_EG_VCF_ATTACK_UP,
	BTN_EG_VCF_DECAY_DOWN,
	BTN_EG_VCF_DECAY_UP,
	BTN_EG_VCF_SUSTAIN_DOWN,
	BTN_EG_VCF_SUSTAIN_UP,
	BTN_EG_VCF_RELEASE_DOWN,
	BTN_EG_VCF_RELEASE_UP,

	// amplifier
	BTN_SYNTH_VOLUME_DOWN,
	BTN_SYNTH_VOLUME_UP,
	BTN_LFO_VCA_WAVEFORM_DOWN,
	BTN_LFO_VCA_WAVEFORM_UP,
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

	// reverb
	BTN_REVERB_DECAY_DOWN,
	BTN_REVERB_DECAY_UP,
	BTN_REVERB_VOLUME_DOWN,
	BTN_REVERB_VOLUME_UP,

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
	BTN_SAVE,

	// help
	BTN_HELP
};

CMainWindow *CMainWindow::s_pThis = 0;

CMainWindow::CMainWindow (CMiniSynthesizer *pSynthesizer, CSynthConfig *pConfig)
:	m_pSynthesizer (pSynthesizer),
	m_pConfig (pConfig),
	m_LFOVCOWaveform (&m_Window, LFOVCOWaveform, pConfig),
	m_LFOVCOFrequency (&m_Window, LFOVCOFrequency, pConfig),
	m_VCOWaveform (&m_Window, VCOWaveform, pConfig),
	m_VCOModulationVolume (&m_Window, VCOModulationVolume, pConfig),
	m_LFOVCFWaveform (&m_Window, LFOVCFWaveform, pConfig),
	m_LFOVCFFrequency (&m_Window, LFOVCFFrequency, pConfig),
	m_VCFCutoffFrequency (&m_Window, VCFCutoffFrequency, pConfig),
	m_VCFResonance (&m_Window, VCFResonance, pConfig),
	m_EGVCFAttack (&m_Window, EGVCFAttack, pConfig),
	m_EGVCFDecay (&m_Window, EGVCFDecay, pConfig),
	m_EGVCFSustain (&m_Window, EGVCFSustain, pConfig),
	m_EGVCFRelease (&m_Window, EGVCFRelease, pConfig),
	m_VCFModulationVolume (&m_Window, VCFModulationVolume, pConfig),
	m_LFOVCAWaveform (&m_Window, LFOVCAWaveform, pConfig),
	m_LFOVCAFrequency (&m_Window, LFOVCAFrequency, pConfig),
	m_EGVCAAttack (&m_Window, EGVCAAttack, pConfig),
	m_EGVCADecay (&m_Window, EGVCADecay, pConfig),
	m_EGVCASustain (&m_Window, EGVCASustain, pConfig),
	m_EGVCARelease (&m_Window, EGVCARelease, pConfig),
	m_VCAModulationVolume (&m_Window, VCAModulationVolume, pConfig),
	m_SynthVolume (&m_Window, SynthVolume, pConfig),
	m_ReverbDecay (&m_Window, ReverbDecay, pConfig),
	m_ReverbVolume (&m_Window, ReverbVolume, pConfig),
	m_pActivePatch (m_pConfig->GetActivePatch ()),
	m_bShowHelp (FALSE)
{
	assert (s_pThis == 0);
	s_pThis = this;

	// create window
	UG_WindowCreate (&m_Window, m_ObjectList, s_ObjectCount, CallbackStub);
	UG_WindowSetStyle (&m_Window, WND_STYLE_2D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize (&m_Window, 0, 0, 799, 479);
	UG_WindowSetBackColor (&m_Window, BACK_COLOR);

	// create controls
	UG_TextboxCreate (&m_Window, &m_Textbox1, TXB_TITLE, 5, 450, 194, 474);
	UG_TextboxCreate (&m_Window, &m_Textbox2, TXB_SUBTITLE, 550, 450, 794, 474);
	// headers
	UG_TextboxCreate (&m_Window, &m_Textbox3, TXB_OSCILLATOR, 5, 5, 194, 25);
	UG_TextboxCreate (&m_Window, &m_Textbox4, TXB_FILTER, 205, 5, 394, 25);
	UG_TextboxCreate (&m_Window, &m_Textbox5, TXB_AMPLIFIER, 405, 5, 594, 25);
	UG_TextboxCreate (&m_Window, &m_Textbox16, TXB_EFFECTS, 5, 300, 194, 325);
	UG_TextboxCreate (&m_Window, &m_Textbox6, TXB_PATCHES, 605, 5, 794, 25);
	// oscillator
	UG_TextboxCreate (&m_Window, &m_Textbox7, TXB_VCO, 5, 30, 194, 55);
	m_VCOWaveform.Create (TXB_VCO_WAVEFORM, BTN_VCO_WAVEFORM_DOWN, BTN_VCO_WAVEFORM_UP, 10, 60);
	UG_TextboxCreate (&m_Window, &m_Textbox8, TXB_LFO_VCO, 5, 150, 194, 175);
	m_LFOVCOWaveform.Create (TXB_LFO_VCO_WAVEFORM, BTN_LFO_VCO_WAVEFORM_DOWN, BTN_LFO_VCO_WAVEFORM_UP, 10, 180);
	m_LFOVCOFrequency.Create (TXB_LFO_VCO_FREQ, BTN_LFO_VCO_FREQ_DOWN, BTN_LFO_VCO_FREQ_UP, 10, 210);
	m_VCOModulationVolume.Create (TXB_VCO_MODULATION_VOLUME, BTN_VCO_MODULATION_VOLUME_DOWN, BTN_VCO_MODULATION_VOLUME_UP, 10, 240);
	// filter
	UG_TextboxCreate (&m_Window, &m_Textbox9, TXB_VCF, 205, 30, 394, 55);
	m_VCFCutoffFrequency.Create (TXB_VCF_CUTOFF_FREQ, BTN_VCF_CUTOFF_FREQ_DOWN, BTN_VCF_CUTOFF_FREQ_UP, 210, 60);
	m_VCFResonance.Create (TXB_VCF_RESONANCE, BTN_VCF_RESONANCE_DOWN, BTN_VCF_RESONANCE_UP, 210, 90);
	UG_TextboxCreate (&m_Window, &m_Textbox10, TXB_LFO_VCF, 205, 150, 394, 175);
	m_LFOVCFWaveform.Create (TXB_LFO_VCF_WAVEFORM, BTN_LFO_VCF_WAVEFORM_DOWN, BTN_LFO_VCF_WAVEFORM_UP, 210, 180);
	m_LFOVCFFrequency.Create (TXB_LFO_VCF_FREQ, BTN_LFO_VCF_FREQ_DOWN, BTN_LFO_VCF_FREQ_UP, 210, 210);
	m_VCFModulationVolume.Create (TXB_VCF_MODULATION_VOLUME, BTN_VCF_MODULATION_VOLUME_DOWN, BTN_VCF_MODULATION_VOLUME_UP, 210, 240);
	UG_TextboxCreate (&m_Window, &m_Textbox11, TXB_EG_VCF, 205, 300, 394, 325);
	m_EGVCFAttack.Create (TXB_EG_VCF_ATTACK, BTN_EG_VCF_ATTACK_DOWN, BTN_EG_VCF_ATTACK_UP, 210, 330);
	m_EGVCFDecay.Create (TXB_EG_VCF_DECAY, BTN_EG_VCF_DECAY_DOWN, BTN_EG_VCF_DECAY_UP, 210, 360);
	m_EGVCFSustain.Create (TXB_EG_VCF_SUSTAIN, BTN_EG_VCF_SUSTAIN_DOWN, BTN_EG_VCF_SUSTAIN_UP, 210, 390);
	m_EGVCFRelease.Create (TXB_EG_VCF_RELEASE, BTN_EG_VCF_RELEASE_DOWN, BTN_EG_VCF_RELEASE_UP, 210, 420);
	// amplifier
	UG_TextboxCreate (&m_Window, &m_Textbox12, TXB_MASTER_VOLUME, 405, 30, 594, 55);
	m_SynthVolume.Create (TXB_SYNTH_VOLUME, BTN_SYNTH_VOLUME_DOWN, BTN_SYNTH_VOLUME_UP, 410, 60);
	UG_TextboxCreate (&m_Window, &m_Textbox13, TXB_LFO_VCA, 405, 150, 594, 175);
	m_LFOVCAWaveform.Create (TXB_LFO_VCA_WAVEFORM, BTN_LFO_VCA_WAVEFORM_DOWN, BTN_LFO_VCA_WAVEFORM_UP, 410, 180);
	m_LFOVCAFrequency.Create (TXB_LFO_VCA_FREQ, BTN_LFO_VCA_FREQ_DOWN, BTN_LFO_VCA_FREQ_UP, 410, 210);
	m_VCAModulationVolume.Create (TXB_VCA_MODULATION_VOLUME, BTN_VCA_MODULATION_VOLUME_DOWN, BTN_VCA_MODULATION_VOLUME_UP, 410, 240);
	UG_TextboxCreate (&m_Window, &m_Textbox14, TXB_EG_VCA, 405, 300, 594, 325);
	m_EGVCAAttack.Create (TXB_EG_VCA_ATTACK, BTN_EG_VCA_ATTACK_DOWN, BTN_EG_VCA_ATTACK_UP, 410, 330);
	m_EGVCADecay.Create (TXB_EG_VCA_DECAY, BTN_EG_VCA_DECAY_DOWN, BTN_EG_VCA_DECAY_UP, 410, 360);
	m_EGVCASustain.Create (TXB_EG_VCA_SUSTAIN, BTN_EG_VCA_SUSTAIN_DOWN, BTN_EG_VCA_SUSTAIN_UP, 410, 390);
	m_EGVCARelease.Create (TXB_EG_VCA_RELEASE, BTN_EG_VCA_RELEASE_DOWN, BTN_EG_VCA_RELEASE_UP, 410, 420);
	// reverb
	UG_TextboxCreate (&m_Window, &m_Textbox15, TXB_REVERB, 5, 330, 194, 355);
	m_ReverbDecay.Create (TXB_REVERB_DECAY, BTN_REVERB_DECAY_DOWN, BTN_REVERB_DECAY_UP, 10, 360);
	m_ReverbVolume.Create (TXB_REVERB_VOLUME, BTN_REVERB_VOLUME_DOWN, BTN_REVERB_VOLUME_UP, 10, 390);
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
	// help
	UG_ButtonCreate (&m_Window, &m_Button13, BTN_HELP, 30, 420, 169, 445);

	// Title
	UG_TextboxSetFont (&m_Window, TXB_TITLE, &FONT_10X16);
	UG_TextboxSetText (&m_Window, TXB_TITLE, "MiniSynth Pi");
	UG_TextboxSetBackColor (&m_Window, TXB_TITLE, TITLE_BACK_COLOR);
	UG_TextboxSetForeColor (&m_Window, TXB_TITLE, TITLE_FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_TITLE, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_SUBTITLE, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_SUBTITLE, "VIRTUAL ANALOG SYNTHESIZER");
	UG_TextboxSetForeColor (&m_Window, TXB_SUBTITLE, FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_SUBTITLE, ALIGN_CENTER_RIGHT);

	// "Oscillator" section
	UG_TextboxSetFont (&m_Window, TXB_OSCILLATOR, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_OSCILLATOR, "OSCILLATOR");
	UG_TextboxSetBackColor (&m_Window, TXB_OSCILLATOR, HEADER_BACK_COLOR);
	UG_TextboxSetForeColor (&m_Window, TXB_OSCILLATOR, HEADER_FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_OSCILLATOR, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_VCO, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_VCO, "VCO");
	UG_TextboxSetForeColor (&m_Window, TXB_VCO, FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_VCO, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_LFO_VCO, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_LFO_VCO, "LFO");
	UG_TextboxSetForeColor (&m_Window, TXB_LFO_VCO, FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_LFO_VCO, ALIGN_CENTER);

	// "Filter" section
	UG_TextboxSetFont (&m_Window, TXB_FILTER, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_FILTER, "FILTER");
	UG_TextboxSetBackColor (&m_Window, TXB_FILTER, HEADER_BACK_COLOR);
	UG_TextboxSetForeColor (&m_Window, TXB_FILTER, HEADER_FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_FILTER, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_VCF, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_VCF, "VCF");
	UG_TextboxSetForeColor (&m_Window, TXB_VCF, FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_VCF, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_LFO_VCF, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_LFO_VCF, "LFO");
	UG_TextboxSetForeColor (&m_Window, TXB_LFO_VCF, FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_LFO_VCF, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_EG_VCF, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_EG_VCF, "ENVELOPE");
	UG_TextboxSetForeColor (&m_Window, TXB_EG_VCF, FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_EG_VCF, ALIGN_CENTER);

	// "Amplifier" section
	UG_TextboxSetFont (&m_Window, TXB_AMPLIFIER, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_AMPLIFIER, "AMPLIFIER");
	UG_TextboxSetBackColor (&m_Window, TXB_AMPLIFIER, HEADER_BACK_COLOR);
	UG_TextboxSetForeColor (&m_Window, TXB_AMPLIFIER, HEADER_FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_AMPLIFIER, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_MASTER_VOLUME, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_MASTER_VOLUME, "MASTER VOLUME");
	UG_TextboxSetForeColor (&m_Window, TXB_MASTER_VOLUME, FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_MASTER_VOLUME, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_LFO_VCA, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_LFO_VCA, "LFO");
	UG_TextboxSetForeColor (&m_Window, TXB_LFO_VCA, FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_LFO_VCA, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_EG_VCA, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_EG_VCA, "ENVELOPE");
	UG_TextboxSetForeColor (&m_Window, TXB_EG_VCA, FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_EG_VCA, ALIGN_CENTER);

	// "Effects" section
	UG_TextboxSetFont (&m_Window, TXB_EFFECTS, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_EFFECTS, "EFFECTS");
	UG_TextboxSetBackColor (&m_Window, TXB_EFFECTS, HEADER_BACK_COLOR);
	UG_TextboxSetForeColor (&m_Window, TXB_EFFECTS, HEADER_FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_EFFECTS, ALIGN_CENTER);

	UG_TextboxSetFont (&m_Window, TXB_REVERB, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_REVERB, "REVERB");
	UG_TextboxSetForeColor (&m_Window, TXB_REVERB, FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_REVERB, ALIGN_CENTER);

	// "PATCHES" section
	UG_TextboxSetFont (&m_Window, TXB_PATCHES, &FONT_8X14);
	UG_TextboxSetText (&m_Window, TXB_PATCHES, "PATCHES");
	UG_TextboxSetBackColor (&m_Window, TXB_PATCHES, HEADER_BACK_COLOR);
	UG_TextboxSetForeColor (&m_Window, TXB_PATCHES, HEADER_FORE_COLOR);
	UG_TextboxSetAlignment (&m_Window, TXB_PATCHES, ALIGN_CENTER);

	UG_ButtonSetFont (&m_Window, BTN_PATCH_0, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_0, "0");
	UG_ButtonSetBackColor (&m_Window, BTN_PATCH_0, BUTTON_HIGH_COLOR);
	UG_ButtonSetFont (&m_Window, BTN_PATCH_1, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_1, "1");
	UG_ButtonSetBackColor (&m_Window, BTN_PATCH_1, BUTTON_BACK_COLOR);
	UG_ButtonSetFont (&m_Window, BTN_PATCH_2, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_2, "2");
	UG_ButtonSetBackColor (&m_Window, BTN_PATCH_2, BUTTON_BACK_COLOR);
	UG_ButtonSetFont (&m_Window, BTN_PATCH_3, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_3, "3");
	UG_ButtonSetBackColor (&m_Window, BTN_PATCH_3, BUTTON_BACK_COLOR);
	UG_ButtonSetFont (&m_Window, BTN_PATCH_4, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_4, "4");
	UG_ButtonSetBackColor (&m_Window, BTN_PATCH_4, BUTTON_BACK_COLOR);
	UG_ButtonSetFont (&m_Window, BTN_PATCH_5, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_5, "5");
	UG_ButtonSetBackColor (&m_Window, BTN_PATCH_5, BUTTON_BACK_COLOR);
	UG_ButtonSetFont (&m_Window, BTN_PATCH_6, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_6, "6");
	UG_ButtonSetBackColor (&m_Window, BTN_PATCH_6, BUTTON_BACK_COLOR);
	UG_ButtonSetFont (&m_Window, BTN_PATCH_7, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_7, "7");
	UG_ButtonSetBackColor (&m_Window, BTN_PATCH_7, BUTTON_BACK_COLOR);
	UG_ButtonSetFont (&m_Window, BTN_PATCH_8, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_8, "8");
	UG_ButtonSetBackColor (&m_Window, BTN_PATCH_8, BUTTON_BACK_COLOR);
	UG_ButtonSetFont (&m_Window, BTN_PATCH_9, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_PATCH_9, "9");
	UG_ButtonSetBackColor (&m_Window, BTN_PATCH_9, BUTTON_BACK_COLOR);
	UG_ButtonSetFont (&m_Window, BTN_LOAD, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_LOAD, "LOAD");
	UG_ButtonSetBackColor (&m_Window, BTN_LOAD, BUTTON_BACK_COLOR);
	UG_ButtonSetFont (&m_Window, BTN_SAVE, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_SAVE, "SAVE");
	UG_ButtonSetBackColor (&m_Window, BTN_SAVE, BUTTON_BACK_COLOR);

	// "Help"
	UG_ButtonSetFont (&m_Window, BTN_HELP, &FONT_8X14);
	UG_ButtonSetText (&m_Window, BTN_HELP, "HELP");
	UG_ButtonSetBackColor (&m_Window, BTN_HELP, BUTTON_BACK_COLOR);
	UpdateAllParameters ();

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
		case BTN_LFO_VCO_WAVEFORM_DOWN:
		case BTN_LFO_VCO_WAVEFORM_UP:
		case BTN_LFO_VCO_FREQ_DOWN:
		case BTN_LFO_VCO_FREQ_UP:
		case BTN_VCO_MODULATION_VOLUME_DOWN:
		case BTN_VCO_MODULATION_VOLUME_UP:
		// filter
		case BTN_VCF_CUTOFF_FREQ_DOWN:
		case BTN_VCF_CUTOFF_FREQ_UP:
		case BTN_VCF_RESONANCE_DOWN:
		case BTN_VCF_RESONANCE_UP:
		case BTN_LFO_VCF_WAVEFORM_DOWN:
		case BTN_LFO_VCF_WAVEFORM_UP:
		case BTN_LFO_VCF_FREQ_DOWN:
		case BTN_LFO_VCF_FREQ_UP:
		case BTN_VCF_MODULATION_VOLUME_DOWN:
		case BTN_VCF_MODULATION_VOLUME_UP:
		case BTN_EG_VCF_ATTACK_DOWN:
		case BTN_EG_VCF_ATTACK_UP:
		case BTN_EG_VCF_DECAY_DOWN:
		case BTN_EG_VCF_DECAY_UP:
		case BTN_EG_VCF_SUSTAIN_DOWN:
		case BTN_EG_VCF_SUSTAIN_UP:
		case BTN_EG_VCF_RELEASE_DOWN:
		case BTN_EG_VCF_RELEASE_UP:
		// amplifier
		case BTN_SYNTH_VOLUME_DOWN:
		case BTN_SYNTH_VOLUME_UP:
		case BTN_LFO_VCA_WAVEFORM_DOWN:
		case BTN_LFO_VCA_WAVEFORM_UP:
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
		// reverb
		case BTN_REVERB_DECAY_DOWN:
		case BTN_REVERB_DECAY_UP:
		case BTN_REVERB_VOLUME_DOWN:
		case BTN_REVERB_VOLUME_UP:
			if (   // oscillator
			       m_LFOVCOWaveform.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_LFOVCOFrequency.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_VCOWaveform.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_VCOModulationVolume.ButtonPressed (nButtonID, m_bShowHelp)
				// filter
			    || m_VCFCutoffFrequency.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_VCFResonance.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_LFOVCFWaveform.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_LFOVCFFrequency.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_VCFModulationVolume.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_EGVCFAttack.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_EGVCFDecay.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_EGVCFSustain.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_EGVCFRelease.ButtonPressed (nButtonID, m_bShowHelp)
			       // amplifier
			    || m_SynthVolume.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_LFOVCAWaveform.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_LFOVCAFrequency.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_VCAModulationVolume.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_EGVCAAttack.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_EGVCADecay.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_EGVCASustain.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_EGVCARelease.ButtonPressed (nButtonID, m_bShowHelp)
				// reverb
			    || m_ReverbDecay.ButtonPressed (nButtonID, m_bShowHelp)
			    || m_ReverbVolume.ButtonPressed (nButtonID, m_bShowHelp))
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
					       BUTTON_BACK_COLOR);
			UG_ButtonSetBackColor (&m_Window, nButtonID, BUTTON_HIGH_COLOR);

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

		case BTN_HELP:
			m_bShowHelp = !m_bShowHelp;
			UG_ButtonSetBackColor (&m_Window, BTN_HELP,
					       m_bShowHelp ? BUTTON_HIGH_COLOR : BUTTON_BACK_COLOR);
			UpdateAllParameters ();
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

void CMainWindow::UpdateAllParameters (boolean bUpdatePatch)
{
	// oscillator
	m_LFOVCOWaveform.Update (m_bShowHelp);
	m_LFOVCOFrequency.Update (m_bShowHelp);
	m_VCOWaveform.Update (m_bShowHelp);
	m_VCOModulationVolume.Update (m_bShowHelp);

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
			UG_ButtonSetBackColor (&m_Window, BTN_PATCH_0+i,   i == nActivePatch
									 ? BUTTON_HIGH_COLOR
									 : BUTTON_BACK_COLOR);
		}
	}
}
