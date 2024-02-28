//
// kernel.cpp
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
#include "kernel.h"
#include "mainwindow.h"
#include "config.h"
#include <circle/machineinfo.h>
#include <circle/string.h>
#include <circle/util.h>
#include <assert.h>

static const char FromKernel[] = "kernel";

CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel (), &m_Timer),
	m_I2CMaster (CMachineInfo::Get ()->GetDevice (DeviceI2CMaster), TRUE),
	m_USBHCI (&m_Interrupt, &m_Timer, TRUE),
	m_EMMC (&m_Interrupt, &m_Timer, &m_ActLED),
	m_GUI (&m_Screen, &m_Interrupt),
	m_Config (&m_FileSystem),
	m_pSynthesizer (0)
{
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;

	if (bOK)
	{
		bOK = m_Screen.Initialize ();
	}

	if (bOK)
	{
		static const char Msg[] = "Starting ...\n";
		m_Screen.Write (Msg, sizeof Msg-1);
	}

#if 0
	if (bOK)
	{
		bOK = m_Serial.Initialize (115200);
	}
#endif

	if (bOK)
	{
		CDevice *pTarget = m_DeviceNameService.GetDevice (m_Options.GetLogDevice (), FALSE);
		if (pTarget == 0)
		{
			pTarget = &m_Screen;
		}

		bOK = m_Logger.Initialize (pTarget);
	}

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}

	if (bOK)
	{
		bOK = m_I2CMaster.Initialize ();
	}

	if (bOK)
	{
		bOK = m_USBHCI.Initialize ();
	}

	if (bOK)
	{
		bOK = m_EMMC.Initialize ();
	}

	if (bOK)
	{
		m_RPiTouchScreen.Initialize ();

		bOK = m_GUI.Initialize ();
	}

	if (bOK)
	{
#if RASPPI <= 4
		const char *pSoundDevice = m_Options.GetSoundDevice ();
		assert (pSoundDevice);
		if (strcmp (pSoundDevice, "sndi2s") == 0)
		{
			m_pSynthesizer = new CMiniSynthesizerI2S (&m_Config, &m_Interrupt,
								  &m_I2CMaster);
		}
#if RASPPI >= 4
		else if (strcmp (pSoundDevice, "sndusb") == 0)
		{
			m_pSynthesizer = new CMiniSynthesizerUSB (&m_Config, &m_Interrupt);
		}
#endif
		else
		{
			m_pSynthesizer = new CMiniSynthesizerPWM (&m_Config, &m_Interrupt);
		}
#else
		m_pSynthesizer = new CMiniSynthesizerUSB (&m_Config, &m_Interrupt);
#endif

		assert (m_pSynthesizer);
		bOK = m_pSynthesizer->Initialize ();
	}

	return bOK;
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);

	// Mount file system
	if (f_mount (&m_FileSystem, DRIVE, 1) != FR_OK)
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot mount drive: %s", DRIVE);
	}

	// Load global configuration
	m_Config.Load ();

	// Load all patches
	for (unsigned i = 0; i < PATCHES; i++)
	{
		m_Config.SetActivePatchNumber (i);

		CPatch *pPatch = m_Config.GetActivePatch ();
		assert (pPatch != 0);

		pPatch->Load ();
	}

	// Activate patch 0
	m_Config.SetActivePatchNumber (0);
	assert (m_pSynthesizer);
	m_pSynthesizer->SetPatch (m_Config.GetActivePatch ());

	m_pSynthesizer->Start ();

	CMainWindow MainWindow (m_pSynthesizer, &m_Config);
	m_GUI.Update (FALSE);

#ifndef SCREENSHOT_AFTER_SECS
	while (m_pSynthesizer->IsActive ())
#else
	while (m_Timer.GetUptime () < SCREENSHOT_AFTER_SECS)
#endif
	{
		boolean bUpdated = m_USBHCI.UpdatePlugAndPlay ();

		m_pSynthesizer->Process (bUpdated);

		if (m_pSynthesizer->ConfigUpdated ())
		{
			MainWindow.UpdateAllParameters (TRUE);
		}

		m_GUI.Update (bUpdated);

		m_CPUThrottle.Update ();

#ifdef SHOW_STATUS
		MainWindow.UpdateStatus (m_pSynthesizer->GetStatus ());
#endif
	}

#ifdef SCREENSHOT_AFTER_SECS
	SaveScreenshot ();
#endif

	return ShutdownHalt;
}

#ifdef SCREENSHOT_AFTER_SECS

void CKernel::SaveScreenshot (void)
{
	CBcmFrameBuffer *pFrameBuffer = m_Screen.GetFrameBuffer ();

	for (unsigned i = 0; i < 20; i++)
	{
		CString Filename;
		Filename.Format ("%sscreenshot%ux%ux%u.%03u", DRIVE, pFrameBuffer->GetWidth (),
				pFrameBuffer->GetHeight (), pFrameBuffer->GetDepth (), i);

		FIL File;
		if (f_open (&File, Filename, FA_WRITE | FA_CREATE_NEW) == FR_OK)
		{
			unsigned nBytesWritten;
			f_write (&File, (void *) (uintptr) pFrameBuffer->GetBuffer (),
				pFrameBuffer->GetSize (), &nBytesWritten);

			f_close (&File);

			return;
		}
	}
}

#endif
