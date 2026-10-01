// Herdr agent controls and status shared by the Miryoku ZSA keymaps.

#pragma once
#ifdef HERDR_AGENT_ENABLE

#include "quantum.h"

void    herdr_init(void);
bool    process_record_herdr(uint16_t keycode, keyrecord_t *record);

bool    herdr_is_connected(void);

#endif
