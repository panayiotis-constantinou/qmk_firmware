// QMK user hooks for this fork's additions to Miryoku: OS-aware clipboard keys,
// the mouse jiggler, the layer colors key, and the Herdr layer.

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"
#include "mouse_jiggler.h"
#include "herdr.h"
#include "os_detection.h"

typedef enum { OS_AUTO = 0, OS_FORCE_MAC = 1, OS_FORCE_LINUX = 2 } os_override_t;
#define OS_OVERRIDE_MASK 0x03 // eeconfig user bits 0-1; Herdr uses higher bits

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
    os_override = (os_override_t)(eeconfig_read_user() & OS_OVERRIDE_MASK);
#ifdef HERDR_AGENT_ENABLE
    herdr_init();
#endif
#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_enable();
#endif
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef RGB_LAYER_COLORS
    if (keycode == RGB_LAYER_COLORS) {
        if (record->event.pressed) {
            rgb_matrix_enable();
            rgb_matrix_set_flags(LED_FLAG_ALL);
            rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            keyboard_config.rgb_matrix_enable = true;
            keyboard_config.disable_layer_led = false;
            eeconfig_update_kb(keyboard_config.raw);
        }
        return false;
    }
#endif
#ifdef HERDR_AGENT_ENABLE
    if (!process_record_herdr(keycode, record)) {
        return false;
    }
#endif
    if (!process_record_mouse_jiggler(keycode, record)) {
        return false;
    }
    // Cycle the OS override: AUTO -> FORCE_MAC -> FORCE_LINUX -> AUTO
    if (keycode == U_OS_CYCLE) {
        if (record->event.pressed) {
            os_override = (os_override_t)((os_override + 1) % 3);
            eeconfig_update_user((eeconfig_read_user() & ~OS_OVERRIDE_MASK) | os_override);
        }
        return false;
    }
    // Miryoku's clipboard keys send the CUA shortcuts (Ctrl/Shift+Insert,
    // Shift+Delete, Undo, Again) by default, which macOS and iPadOS ignore; in
    // Mac mode send the Cmd shortcuts instead.
    if (is_mac_mode()) {
        switch (keycode) {
            case U_CPY: if (record->event.pressed) tap_code16(LGUI(KC_C)); return false;
            case U_CUT: if (record->event.pressed) tap_code16(LGUI(KC_X)); return false;
            case U_PST: if (record->event.pressed) tap_code16(LGUI(KC_V)); return false;
            case U_UND: if (record->event.pressed) tap_code16(LGUI(KC_Z)); return false;
            case U_RDO: if (record->event.pressed) tap_code16(SCMD(KC_Z)); return false;
        }
    }
    return true;
}

void housekeeping_task_user(void) {
    housekeeping_task_mouse_jiggler();
#ifdef HERDR_AGENT_ENABLE
    herdr_task();
#endif
}
