// Copyright 2022 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"
#include "mouse_jiggler.h"


// Additional Features double tap guard

enum {
    U_TD_BOOT,
#define MIRYOKU_X(LAYER, STRING) U_TD_U_##LAYER,
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

void u_td_fn_boot(tap_dance_state_t *state, void *user_data) {
  if (state->count == 2) {
    reset_keyboard();
  }
}

#define MIRYOKU_X(LAYER, STRING) \
void u_td_fn_U_##LAYER(tap_dance_state_t *state, void *user_data) { \
  if (state->count == 2) { \
    default_layer_set((layer_state_t)1 << U_##LAYER); \
  } \
}
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X

tap_dance_action_t tap_dance_actions[] = {
    [U_TD_BOOT] = ACTION_TAP_DANCE_FN(u_td_fn_boot),
#define MIRYOKU_X(LAYER, STRING) [U_TD_U_##LAYER] = ACTION_TAP_DANCE_FN(u_td_fn_U_##LAYER),
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};


// keymap

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
#define MIRYOKU_X(LAYER, STRING) [U_##LAYER] = U_MACRO_VA_ARGS(MIRYOKU_LAYERMAPPING_##LAYER, MIRYOKU_LAYER_##LAYER),
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};


// shift functions

const key_override_t capsword_key_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);

const key_override_t *key_overrides[] = {
    &capsword_key_override,
    NULL
};


// thumb combos

#if defined (MIRYOKU_KLUDGE_THUMBCOMBOS)
const uint16_t PROGMEM thumbcombos_base_right[] = {LT(U_SYM, KC_ENT), LT(U_NUM, KC_BSPC), COMBO_END};
const uint16_t PROGMEM thumbcombos_base_left[] = {LT(U_NAV, KC_SPC), LT(U_MOUSE, KC_TAB), COMBO_END};
const uint16_t PROGMEM thumbcombos_nav[] = {KC_ENT, KC_BSPC, COMBO_END};
const uint16_t PROGMEM thumbcombos_mouse[] = {MS_BTN2, MS_BTN1, COMBO_END};
const uint16_t PROGMEM thumbcombos_media[] = {KC_MSTP, KC_MPLY, COMBO_END};
const uint16_t PROGMEM thumbcombos_num[] = {KC_0, KC_MINS, COMBO_END};
  #if defined (MIRYOKU_LAYERS_FLIP)
const uint16_t PROGMEM thumbcombos_sym[] = {KC_UNDS, KC_LPRN, COMBO_END};
  #else
const uint16_t PROGMEM thumbcombos_sym[] = {KC_RPRN, KC_UNDS, COMBO_END};
  #endif
const uint16_t PROGMEM thumbcombos_fun[] = {KC_SPC, KC_TAB, COMBO_END};
combo_t key_combos[COMBO_COUNT] = {
  COMBO(thumbcombos_base_right, LT(U_FUN, KC_DEL)),
  COMBO(thumbcombos_base_left, LT(U_MEDIA, KC_ESC)),
  COMBO(thumbcombos_nav, KC_DEL),
  COMBO(thumbcombos_mouse, MS_BTN3),
  COMBO(thumbcombos_media, KC_MUTE),
  COMBO(thumbcombos_num, KC_DOT),
  #if defined (MIRYOKU_LAYERS_FLIP)
  COMBO(thumbcombos_sym, KC_RPRN),
  #else
  COMBO(thumbcombos_sym, KC_LPRN),
  #endif
  COMBO(thumbcombos_fun, KC_APP)
};
#endif


// OS detection + clipboard remapping + mouse jiggler (shared across all keyboards)

#include "os_detection.h"

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
#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_enable();
#endif
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_mouse_jiggler(keycode, record)) {
        return false;
    }
    // QK_OUTPUT_AUTO cycles OS override: AUTO -> FORCE_MAC -> FORCE_LINUX -> AUTO
    if (keycode == QK_OUTPUT_AUTO) {
        if (record->event.pressed) {
            os_override = (os_override_t)((os_override + 1) % 3);
            eeconfig_update_user((uint32_t)os_override);
        }
        return false;
    }
    // In Mac mode: intercept Ctrl+clipboard keys and send Cmd instead
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

void housekeeping_task_user(void) {
    housekeeping_task_mouse_jiggler();
}
