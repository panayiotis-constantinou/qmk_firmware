# Copyright 2019 Manna Harbour
# https://github.com/manna-harbour/miryoku

OS_DETECTION_ENABLE = yes
OS_DETECTION_SINGLE_REPORT = yes
SRC += users/manna-harbour_miryoku/user_hooks.c
SRC += users/manna-harbour_miryoku/mouse_jiggler.c

ifeq ($(strip $(RGB_MATRIX_ENABLE)), yes)
  SRC += users/manna-harbour_miryoku/miryoku_rgb.c
endif

ifeq ($(strip $(HERDR_AGENT_ENABLE)), yes)
  COMBO_ENABLE = yes
  MIDI_ENABLE = yes
  OPT_DEFS += -DHERDR_AGENT_ENABLE
  SRC += users/manna-harbour_miryoku/herdr.c
endif
