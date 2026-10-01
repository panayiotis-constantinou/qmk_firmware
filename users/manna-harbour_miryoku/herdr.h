// Herdr agent controls and status shared by the Miryoku ZSA keymaps.

#pragma once
#ifdef HERDR_AGENT_ENABLE

#include "quantum.h"

enum herdr_state {
    HERDR_IDLE,
    HERDR_WORKING,
    HERDR_BLOCKED,
    HERDR_DONE,
    HERDR_UNKNOWN,
    HERDR_EMPTY = 7,
};

#define HERDR_SLOT_COUNT 4

void    herdr_init(void);
bool    process_record_herdr(uint16_t keycode, keyrecord_t *record);

bool    herdr_is_connected(void);
uint8_t herdr_slot_state(uint8_t slot);

#ifdef RGB_MATRIX_ENABLE
void herdr_render_status(uint8_t connection_led, const uint8_t *slot_leds);
#endif

#endif
