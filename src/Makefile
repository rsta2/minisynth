#
# Makefile
#

-include ../Config.mk

CIRCLEHOME ?= ../../circle

CPPFLAGS += -Wno-write-strings

OBJS	= main.o kernel.o minisynth.o \
	  pckeyboard.o \
	  voice.o oscillator.o filter.o amplifier.o envelopegenerator.o \
	  synthconfig.o patch.o parameter.o \
	  mainwindow.o guiparameter.o \
	  math.o

LIBS	= $(CIRCLEHOME)/addon/ugui/libugui.a \
	  $(CIRCLEHOME)/addon/Properties/libproperties.a \
	  $(CIRCLEHOME)/addon/SDCard/libsdcard.a \
	  $(CIRCLEHOME)/lib/usb/libusb.a \
	  $(CIRCLEHOME)/lib/input/libinput.a \
	  $(CIRCLEHOME)/lib/fs/fat/libfatfs.a \
	  $(CIRCLEHOME)/lib/fs/libfs.a \
	  $(CIRCLEHOME)/lib/libcircle.a

include $(CIRCLEHOME)/app/Rules.mk
