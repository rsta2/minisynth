MiniSynth Pi
============

> Raspberry Pi is a trademark of the Raspberry Pi Foundation.

> If you read this file in an editor you should switch line wrapping on.

Overview
--------

MiniSynth Pi is a polyphonic virtual analog audio synthesizer, running bare metal (without separate operating system) on the Raspberry Pi. On the Raspberry Pi 2 and 3 it allows to play up to 24 polyphonic voices at a time, on the Raspberry Pi 1 only 4 voices.

You have to attach an USB MIDI keyboard controller (which supports the USB Audio Class MIDI specification) or an USB PC keyboard to your Raspberry Pi to play on it. The audio signal is available on the 3.5mm headphones jack. Thus Raspberry Pi models without headphones jack (e.g. Raspberry Pi Zero) are not supported. The graphical user interface (GUI) of MiniSynth Pi can be controlled using a standard USB mouse or the official Raspberry Pi touch screen.

Getting
-------

Normally you need a *Git* client to get the MiniSynth Pi source code. Go to the directory where you want to place MiniSynth Pi on your hard disk and enter:

	git clone https://github.com/rsta2/minisynth.git minisynth
	cd minisynth
	git submodule update --init

This will place the source code in the subdirectory *minisynth/* and clones the submodule *circle* into the *minisynth/circle/* subdirectory.

Building
--------

MiniSynth Pi uses the Circle bare metal build environment for the Raspberry Pi. You need an appropriate compiler toolchain for ARM processors to build it. Have a look at the Circle *README.md* file (in *circle/*) for further information on this (section *Building*). The build information herein is for Linux hosts only. For building on other hosts you have to adapt some script files.

When the toolchain is installed on your computer you can build MiniSynth Pi using the following commands:

	./configure 2 arm-none-eabi-
	./makeall clean
	./makeall

The `configure` command writes a *Config.mk* file for Circle and patches Circle, so that it allows to use multiple CPU cores. "2" is the major revision number of your Raspberry Pi (1, 2 or 3). The second (optional) parameter is the prefix of the commands of your toolchain and can be preceded with a path. Do not forget the dash at the end of the prefix!

If the build was successful, you find the executable image file of MiniSynth Pi in the *src/* subdirectory with the name *kernel.img* (Raspberry Pi 1), *kernel7.img* (Raspberry Pi 2) or *kernel8-32.img* (Raspberry Pi 3).

Installation
------------

You will need a SD card with FAT16 or FAT32 partition to install MiniSynth Pi. This partition must be the first file system on the SD card. You have to copy the *kernel???.img* file, built in the previous step, to this FAT partition. All files have to be placed in the root directory of this partition.

Furthermore you need the Raspberry Pi firmware. You can get it as follows:

	cd circle/boot
	make

You have to copy the three files *bootcode.bin*, *start.elf* and *fixup.dat* from the *circle/boot/* subdirectory to the FAT partition on the SD card.

Finally you may copy the configuration files *patch0.txt* (example patch) and *velocity-???.txt* (keyboard velocity curve) from the *config/* subdirectory to the SD card. The appropriate velocity curve file has to be renamed to *velocity.txt* to be used.

Put the SD card into the card reader of your Raspberry Pi.

Using
-----

Before powering on your Raspberry Pi, the following devices have to be attached:

* HDMI display (must support 800x600 pixels mode)
* USB MIDI keyboard controller or USB PC keyboard
* Standard USB mouse (if official touch screen is not used)
* Headphones or amplifier (on the 3.5mm jack)

The USB devices must not be removed or powered off while MiniSynth Pi is running. USB hot-plugging is not supported.

MiniSynth Pi starts in about four seconds. It is controlled using the following GUI:

	+-----------------------------------------------------------+
	|  OSCILLATOR       FILTER       AMPLIFIER       PATCHES    |
	|     VCO            VCF       MASTER VOLUME        0       |
	| <   Wave   >   <  Cutoff  >   <  Volume  >        1       |
	|                < Resonance>                       2       |
	|                                                   3       |
	|     LFO            LFO            LFO             4       |
	| <   Wave   >   <   Wave   >   <   Wave   >        5       |
	| <   Rate   >   <   Rate   >   <   Rate   >        6       |
	| <  Volume  >   <  Volume  >   <  Volume  >        7       |
	|                                                   8       |
	|                  ENVELOPE       ENVELOPE          9       |
	|     HELP       <  Attack  >   <  Attack  >                |
	|                <   Decay  >   <   Decay  >       LOAD     |
	|                <  Sustain >   <  Sustain >       SAVE     |
	|                <  Release >   <  Release >                |
	| MiniSynth Pi                   VIRTUAL ANALOG SYNTHESIZER |
	+-----------------------------------------------------------+

You will get that picture when you click the *HELP* button with the mouse or on the touch screen. By default the GUI does not show the help info, but the values of the different parameters of the currently selected patch (see *Parameters*).

The GUI allows to select ten different sound configurations (patches 0-9). There is always one (highlighted) active patch, which can be edited using the different parameter controls. The parameters of the active patch can be saved to a configuration file on the SD card (*patchX.txt* where X is the number of the patch). On start-up the configuration of all patches are loaded from these files (if available) or initialized to the default preset.

The USB PC keyboard allows playing two octaves (keys C2-C4). Its mapping is as follows:

	   +-----+-----+     +-----+-----+-----+
	   |2 C#2|3 D#2|     |5 F#2|6 G#2|7 A#3|
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|Q C2 |W D2 |E E2 |R F2 |T G2 |Y A3 |U B3 |
	+-----+--+--+--+--+--+--+--+--+--+--+--+--+--+
	         |S C#3|D D#3|     |G F#3|H G#3|J A#4|
	      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+-----+
	      |Z C3 |X D3 |C E3 |V F3 |B G3 |N A4 |M B4 |, C4 |
	      +-----+-----+-----+-----+-----+-----+-----+-----+

	+--------+
	|Key Note|
	+--------+

The inscription of the keyboard may vary, but the physical position of the piano keys is the same on all keyboards. Using this scheme you should be able to find them. Please note that PC keyboards often do not allow to use all voices simultaneously.

A Raspberry Pi running MiniSynth Pi can be powered off at any time when the green activity LED is not on.

Parameters
----------

One patch of MiniSynth Pi has the following parameters:

| Section    | Module   | Parameter | Unit | Range     | Default | Description          |
| ---------- | -------- | --------- | ---- | --------- | ------- | -------------------- |
| OSCILLATOR | VCO      | Wave      |      |           | Square  | Waveform (*)         |
| OSCILLATOR | LFO      | Wave      |      |           | Sine    | Waveform (*)         |
| OSCILLATOR | LFO      | Rate      | Hz   | 1-35      | 20      | Modulation frequency |
| OSCILLATOR | LFO      | Volume    | %    | 0-100     | 0       | Modulation volume    |
| FILTER     | VCF      | Cutoff    | %    | 10-100    | 80      | Cutoff frequency     |
| FILTER     | VCF      | Resonance | %    | 0-100     | 50      | Resonance parameter  |
| FILTER     | LFO      | Wave      |      |           | Sine    | Waveform (*)         |
| FILTER     | LFO      | Rate      | Hz   | 1-35      | 20      | Modulation frequency |
| FILTER     | LFO      | Volume    | %    | 0-90      | 0       | Modulation volume    |
| FILTER     | ENVELOPE | Attack    | ms   | 0-2000    | 0       | Attack delay         |
| FILTER     | ENVELOPE | Decay     | ms   | 100-10000 | 4000    | Decay delay          |
| FILTER     | ENVELOPE | Sustain   | %    | 0-100     | 100     | Sustain level        |
| FILTER     | ENVELOPE | Release   | ms   | 0-5000    | 1000    | Release delay        |
| AMPLIFIER  |          | Volume    | %    | 0-100     | 50      | Master volume        |
| AMPLIFIER  | LFO      | Wave      |      |           | Sine    | Waveform (*)         |
| AMPLIFIER  | LFO      | Rate      | Hz   | 1-35      | 5       | Modulation frequency |
| AMPLIFIER  | LFO      | Volume    | %    | 0-90      | 0       | Modulation volume    |
| AMPLIFIER  | ENVELOPE | Attack    | ms   | 0-2000    | 100     | Attack delay         |
| AMPLIFIER  | ENVELOPE | Decay     | ms   | 100-10000 | 4000    | Decay delay          |
| AMPLIFIER  | ENVELOPE | Sustain   | %    | 0-100     | 100     | Sustain level        |
| AMPLIFIER  | ENVELOPE | Release   | ms   | 0-5000    | 100     | Release delay        |

(*) Waveform can be: Sine, Square, Sawtooth, Triangle, Pulse 12.5% or Pulse 25%

MiniSynth Pi allows to use a specific keyboard velocity curve, which fits best to your keyboard and your playing style. It has to be provided in the file *velocity.txt* on the SD card. The default velocity curve is linear. Have a look into the example files in the *config/* subdirectory. If you want to use one of these files, it has to be renamed to *velocity.txt* on the SD card. It should be easy to modify one example file to adjust the velocity curve to your own needs.

Credits
-------

MiniSynth Pi uses the following source modules:

* [Circle C++ bare metal environment for the Raspberry Pi](https://github.com/rsta2/circle/) (includes USB MIDI driver by Joshua Otto)
* [uGUI library](http://www.embeddedlightning.com/ugui/) by Achim Doebler
* [EMMC SD card driver (part of rpi-boot)](https://github.com/jncronin/rpi-boot/blob/master/emmc.c) by John Cronin
* Optimized expf() function from the [GNU C Library](https://www.gnu.org/software/libc/), Copyright (C) 1997-2017 Free Software Foundation, Inc.

Additional information has been obtained from:

* [Cookbook formulae for audio EQ biquad filter coefficients](http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt) by Robert Bristow-Johnson
* [Digital Synth WRA32](https://github.com/risgk/digital-synth-wra32/) by Ryo Ishigaki
