MiniSynth Pi
============

> Raspberry Pi is a trademark of the Raspberry Pi Foundation.

> If you read this file in an editor you should switch line wrapping on.

Overview
--------

MiniSynth Pi is a virtual analog polyphonic (8 voices) audio synthesizer, running bare metal on the Raspberry Pi. It is currently under development (including this documentation).

You have to attach an USB PC keyboard to your Raspberry Pi to play on it. USB MIDI class support is planned. The audio signal is available on the 3.5mm headphones jack. Thus Raspberry Pi models without headphones jack (e.g. Raspberry Pi Zero) are not supported. The graphical user interface (GUI) of MiniSynth Pi can be controlled using a standard USB mouse or the official Raspberry Pi touch screen.

Using
-----

Before powering on your Raspberry Pi, the following devices have to be attached:

* HDMI display (must support 800x600 pixels mode)
* USB PC keyboard
* Standard USB mouse (if official touch screen is not used)
* Headphones or amplifier (to the 3.5mm jack)

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

The GUI allows to select ten different sound configurations (patches 0-9). There is always one (highlighted) active patch, which can be edited using the different parameter controls. The parameters of the active patch can be saved to a configuration file on the SD card (*patchX.txt* where X is the number of the patch). On start-up the configuration of all patches are loaded from these files (if available) or initialized to a default preset.

The USB PC keyboard allows playing two octaves (keys C3-C5). Its mapping is as follows:

	   +-----+-----+     +-----+-----+-----+
	   |2 C#3|3 D#3|     |5 F#3|6 G#3|7 A#3|
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|Q C3 |W D3 |E E3 |R F3 |T G3 |Y A3 |U B3 |
	+-----+--+--+--+--+--+--+--+--+--+--+--+--+--+
	         |S C#4|D D#4|     |G F#4|H G#4|J A#4|
	      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+-----+
	      |Z C4 |X D4 |C E4 |V F4 |B G4 |N A4 |M B4 |, C5 |
	      +-----+-----+-----+-----+-----+-----+-----+-----+

	+--------+
	|Key Note|
	+--------+

The inscription of the keyboard may vary, but the physical position of the piano keys is the same on all keyboards. Using this scheme you should be able to find them.

MiniSynth Pi can be powered off at any time when the green activity LED is not on.

Credits
-------

MiniSynth Pi uses the following source modules:

* [Circle C++ bare metal environment for the Raspberry Pi](https://github.com/rsta2/circle/)
* [uGUI library](http://www.embeddedlightning.com/ugui/) by Achim Doebler
* [EMMC SD card driver (part of rpi-boot)](https://github.com/jncronin/rpi-boot/blob/master/emmc.c) by John Cronin
