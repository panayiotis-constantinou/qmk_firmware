// generated from users/manna-harbour_miryoku/miryoku.org

#pragma once

// Map Miryoku's 36-key layout onto the Planck EZ's 48-key grid (4x12).
// Middle two columns (cols 5-6) and outer thumb keys are KC_NO.
#define LAYOUT_miryoku( \
       K00,   K01,   K02,   K03,   K04,   K05,   K06,   K07,   K08,   K09, \
       K10,   K11,   K12,   K13,   K14,   K15,   K16,   K17,   K18,   K19, \
       K20,   K21,   K22,   K23,   K24,   K25,   K26,   K27,   K28,   K29, \
       N30,   N31,   K32,   K33,   K34,   K35,   K36,   K37,   N38,   N39  \
) \
LAYOUT_planck_grid( \
  K00,   K01,   K02,   K03,   K04,   KC_NO, KC_NO, K05,   K06,   K07,   K08,   K09,  \
  K10,   K11,   K12,   K13,   K14,   KC_NO, KC_NO, K15,   K16,   K17,   K18,   K19,  \
  K20,   K21,   K22,   K23,   K24,   KC_NO, KC_NO, K25,   K26,   K27,   K28,   K29,  \
  KC_NO, KC_NO, K32,   K33,   K34,   KC_NO, KC_NO, K35,   K36,   K37,   KC_NO, KC_NO \
)

#undef RGB_MATRIX_TIMEOUT

// Keep the small LEDs below the space bar out of layer indication: Miryoku's
// layer numbers don't match the stock scheme, and the sort mode is RGB LED 37.
#define PLANCK_EZ_USER_LEDS

// Media layer: double-tap Q to enter the bootloader; B restores layer colours;
// J toggles LEDs; L/U cycle effects; Y/' adjust speed.
#define RGB_LAYER_COLORS QK_USER_0
#define MIRYOKU_LAYER_MEDIA \
    TD(U_TD_BOOT), TD(U_TD_U_TAP), TD(U_TD_U_EXTRA), TD(U_TD_U_BASE), RGB_LAYER_COLORS, \
    RM_TOGG, RM_NEXT, RM_PREV, RM_SPDU, RM_SPDD, \
    KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, U_NA, \
    U_NU, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT, \
    U_NA, KC_ALGR, TD(U_TD_U_FUN), TD(U_TD_U_MEDIA), U_NA, \
    U_OS_CYCLE, U_NU, U_NU, U_NU, U_NU, \
    U_NP, U_NP, U_NA, U_NA, U_NA, KC_MSTP, KC_MPLY, KC_MUTE, U_NP, U_NP

#define ENABLE_RGB_MATRIX_BREATHING
#define ENABLE_RGB_MATRIX_CYCLE_ALL
#define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#define ENABLE_RGB_MATRIX_RAINDROPS
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE

#define USB_SUSPEND_WAKEUP_DELAY 0
#undef MOUSEKEY_INTERVAL
#define MOUSEKEY_INTERVAL 14
#undef MOUSEKEY_MAX_SPEED
#define MOUSEKEY_MAX_SPEED 11
#undef MOUSEKEY_WHEEL_INTERVAL
#define MOUSEKEY_WHEEL_INTERVAL 85
#define SERIAL_NUMBER "DY9WP/m5Ejj3"
#define LAYER_STATE_16BIT
#define RGB_MATRIX_STARTUP_SPD 60
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR
