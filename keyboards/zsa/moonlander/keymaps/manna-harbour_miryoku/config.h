// generated from users/manna-harbour_miryoku/miryoku.org

#pragma once

// Map Miryoku's 36-key layout onto the Moonlander's 72-key matrix.
// Unused positions are KC_NO.
#define LAYOUT_miryoku( \
       K00,   K01,   K02,   K03,   K04,   K05,   K06,   K07,   K08,   K09, \
       K10,   K11,   K12,   K13,   K14,   K15,   K16,   K17,   K18,   K19, \
       K20,   K21,   K22,   K23,   K24,   K25,   K26,   K27,   K28,   K29, \
       N30,   N31,   K32,   K33,   K34,   K35,   K36,   K37,   N38,   N39  \
) \
LAYOUT_moonlander( \
  KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,   KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, \
  K00,   K01,   K02,   K03,   K04,   KC_NO, KC_NO,   KC_NO, KC_NO, K05,   K06,   K07,   K08,   K09,   \
  K10,   K11,   K12,   K13,   K14,   KC_NO, KC_NO,           KC_NO, KC_NO, K15,   K16,   K17,   K18,   K19,   \
  K20,   K21,   K22,   K23,   K24,   KC_NO,                   KC_NO, K25,   K26,   K27,   K28,   K29,          \
  KC_NO, KC_NO, K32,   K33,   K34,   KC_NO,                   KC_NO, K35,   K36,   K37,   KC_NO, KC_NO,        \
  KC_NO, KC_NO, KC_NO,                                                 KC_NO, KC_NO, KC_NO                       \
)

#undef RGB_MATRIX_TIMEOUT
#define RGB_MATRIX_TIMEOUT 600000
#define USB_SUSPEND_WAKEUP_DELAY 0
#undef MOUSEKEY_INTERVAL
#define MOUSEKEY_INTERVAL 12
#undef MOUSEKEY_MAX_SPEED
#define MOUSEKEY_MAX_SPEED 11
#undef MOUSEKEY_WHEEL_INTERVAL
#define MOUSEKEY_WHEEL_INTERVAL 85
#define SERIAL_NUMBER "9BAVo/ea5DVr"
#define LAYER_STATE_16BIT
#define RGB_MATRIX_STARTUP_SPD 60
