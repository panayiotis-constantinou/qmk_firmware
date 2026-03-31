/* Copyright 2021 weteor
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// generated from users/manna-harbour_miryoku/miryoku.org  -*- buffer-read-only: t -*-

#include "os_detection.h"
#include "mouse_jiggler.h"

typedef enum { OS_AUTO = 0, OS_FORCE_MAC = 1, OS_FORCE_LINUX = 2 } os_override_t;

static os_variant_t  detected_os = OS_UNSURE;
static os_override_t os_override  = OS_AUTO;

static bool is_mac_mode(void) {
    if (os_override == OS_FORCE_MAC)   return true;
    if (os_override == OS_FORCE_LINUX) return false;
    return (detected_os == OS_MACOS || detected_os == OS_IOS);
}

bool process_detected_host_os_user(os_variant_t detected) {
    detected_os = detected;
    return true;
}

void eeconfig_init_user(void) {
    eeconfig_update_user(OS_AUTO);
}

void keyboard_post_init_user(void) {
    os_override = (os_override_t)(eeconfig_read_user() & 0x03);
}

// Hook into process_record_user to handle MJ_TOGG keycode
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_mouse_jiggler(keycode, record)) {
        return false;
    }

    // Repurpose OU_AUTO as OS-mode cycle key: AUTO -> FORCE_MAC -> FORCE_LINUX -> AUTO
    if (keycode == QK_OUTPUT_AUTO) {
        if (record->event.pressed) {
            os_override = (os_override_t)((os_override + 1) % 3);
            eeconfig_update_user((uint32_t)os_override);
        }
        return false;
    }

    // Intercept clipboard keys in Mac mode: substitute Cmd for Ctrl
    if (is_mac_mode()) {
        switch (keycode) {
            case C(KC_C): if (record->event.pressed) tap_code16(LGUI(KC_C)); return false;
            case C(KC_X): if (record->event.pressed) tap_code16(LGUI(KC_X)); return false;
            case C(KC_V): if (record->event.pressed) tap_code16(LGUI(KC_V)); return false;
            case C(KC_Z): if (record->event.pressed) tap_code16(LGUI(KC_Z)); return false;
            case C(KC_Y): if (record->event.pressed) tap_code16(SCMD(KC_Z)); return false;
        }
    }

    return true;
}

// Hook into housekeeping_task_user to perform the jiggling
void housekeeping_task_user(void) {
    housekeeping_task_mouse_jiggler();
}
