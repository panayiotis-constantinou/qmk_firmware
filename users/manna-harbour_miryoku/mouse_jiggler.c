// Mouse Jiggler Implementation
// Based on ZSA's mouse jiggler module
// Copyright 2025 ZSA Technology Labs, Inc <@zsa>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "mouse_jiggler.h"

static bool     mouse_jiggler_enabled = false;
static uint16_t mouse_jiggler_timer   = 0;
static int8_t   jiggle_direction      = 3;

#ifndef MOUSE_JIGGLER_INTERVAL_MS
#    define MOUSE_JIGGLER_INTERVAL_MS 1000
#endif

#ifndef MOUSE_JIGGLER_MOVEMENT
#    define MOUSE_JIGGLER_MOVEMENT 1
#endif

bool mouse_jiggler_is_enabled(void) {
    return mouse_jiggler_enabled;
}

void mouse_jiggler_enable(void) {
    mouse_jiggler_enabled = true;
    mouse_jiggler_timer   = timer_read();
}

void mouse_jiggler_disable(void) {
    mouse_jiggler_enabled = false;
}

void mouse_jiggler_toggle(void) {
    if (mouse_jiggler_enabled) {
        mouse_jiggler_disable();
    } else {
        mouse_jiggler_enable();
    }
}

bool process_record_mouse_jiggler(uint16_t keycode, keyrecord_t *record) {
    if (keycode == MJ_TOGG && record->event.pressed) {
        mouse_jiggler_toggle();
        return false;
    }
    return true;
}

void housekeeping_task_mouse_jiggler(void) {
    if (mouse_jiggler_enabled) {
        if (timer_elapsed(mouse_jiggler_timer) > MOUSE_JIGGLER_INTERVAL_MS) {
            mouse_jiggler_timer = timer_read();

            report_mouse_t report = {0};
            report.x              = MOUSE_JIGGLER_MOVEMENT * jiggle_direction;

            host_mouse_send(&report);

            jiggle_direction = -jiggle_direction;
        }
    }
}
