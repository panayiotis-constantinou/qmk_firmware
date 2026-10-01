# Copyright 2019 Manna Harbour
# https://github.com/manna-harbour/miryoku


ifeq ($(strip $(HERDR_AGENT_ENABLE)), yes)
  MIDI_ENABLE = yes
  OPT_DEFS += -DHERDR_AGENT_ENABLE
  SRC += users/manna-harbour_miryoku/herdr.c
endif
