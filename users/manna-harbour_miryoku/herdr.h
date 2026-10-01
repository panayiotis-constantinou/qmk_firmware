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

// Why a blocked agent is waiting, judged by the bridge.
enum herdr_reason {
    HERDR_REASON_UNKNOWN,
    HERDR_REASON_PERMISSION,
    HERDR_REASON_QUESTION,
    HERDR_REASON_ERROR,
};

#define HERDR_SLOT_COUNT 4

void    herdr_init(void);
bool    process_record_herdr(uint16_t keycode, keyrecord_t *record);

bool    herdr_is_connected(void);
uint8_t herdr_slot_state(uint8_t slot);
bool    herdr_is_armed(uint16_t keycode);
bool    herdr_sounds_are_enabled(void);

#ifdef RGB_MATRIX_ENABLE
void herdr_render_status(uint8_t connection_led, const uint8_t *slot_leds);
void herdr_render_dropped_press(void);
#endif

#endif
