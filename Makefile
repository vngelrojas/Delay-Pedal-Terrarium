# Project Name
TARGET = Pedal

# Sources
CPP_SOURCES = Pedal.cpp \
			  Delay.cpp \
			  ToneFilter.cpp \
			  TapTempo.cpp \

CPP_HEADERS = Delay.h \
			  ToneFilter.h \
			  TapTempo.h \

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR = ../../DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
