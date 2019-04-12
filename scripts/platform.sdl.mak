USE_LIBA = 0
ION_KEYBOARD_LAYOUT = layout_B2
EPSILON_ONBOARDING_APP = 0
EPSILON_GETOPT = 1

SFLAGS += -fPIE
EXE = elf

ifndef MODEL
  $(error MODEL should be defined)
endif

BUILD_DIR := $(BUILD_DIR)/$(MODEL)

include scripts/platform.sdl.$(MODEL).mak
