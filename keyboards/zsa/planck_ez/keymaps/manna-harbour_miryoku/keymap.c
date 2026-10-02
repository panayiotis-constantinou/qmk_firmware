#include QMK_KEYBOARD_H
#include "miryoku_rgb.h"
#include "herdr.h"

// Miryoku key index -> Planck LED index (36 entries).
static const uint8_t miryoku_to_led[MIRYOKU_KEY_COUNT] = {
//  K00  K01  K02  K03  K04    K05  K06  K07  K08  K09
      0,   1,   2,   3,   4,    7,   8,   9,  10,  11,
//  K10  K11  K12  K13  K14    K15  K16  K17  K18  K19
     12,  13,  14,  15,  16,   19,  20,  21,  22,  23,
//  K20  K21  K22  K23  K24    K25  K26  K27  K28  K29
     24,  25,  26,  27,  28,   31,  32,  33,  34,  35,
//  K32  K33  K34    K35  K36  K37
     38,  39,  40,   42,  43,  44,
};

// The center 3x2 block and the four bottom-outer positions have no switches:
// they stay dark except for the Herdr status and the toggle indicators.
static const uint8_t led_only_leds[] = {5, 6, 17, 18, 29, 30, 36, 37, 45, 46};
// Caps Lock at the bottom-left corner, Scroll Lock at the center-block bottom
// left, and mouse jiggler at the bottom-right corner.
static const uint8_t free_to_led[] = {36, 29, 46};
// Agent slots on the center block's top two rows, left to right.
static const uint8_t herdr_slot_leds[HERDR_SLOT_COUNT] = {5, 6, 17, 18};

static const miryoku_leds_t leds = {
    .keys             = miryoku_to_led,
    .free_leds        = free_to_led,
    .free_led_count   = ARRAY_SIZE(free_to_led),
    .led_only         = led_only_leds,
    .led_only_count   = ARRAY_SIZE(led_only_leds),
    .herdr_slots      = herdr_slot_leds,
    .herdr_connection = 41, // the space bar
    .herdr_sort       = 37, // the inner bottom-left LED
};

bool rgb_matrix_indicators_user(void) { return miryoku_rgb_indicators(&leds); }
