#include QMK_KEYBOARD_H
#include "miryoku_rgb.h"
#include "herdr.h"

// Miryoku key index -> Moonlander LED index (36 entries).
static const uint8_t miryoku_to_led[MIRYOKU_KEY_COUNT] = {
//  K00  K01  K02  K03  K04    K05  K06  K07  K08  K09
      1,   6,  11,  16,  21,   57,  52,  47,  42,  37,
//  K10  K11  K12  K13  K14    K15  K16  K17  K18  K19
      2,   7,  12,  17,  22,   58,  53,  48,  43,  38,
//  K20  K21  K22  K23  K24    K25  K26  K27  K28  K29
      3,   8,  13,  18,  23,   59,  54,  49,  44,  39,
//  K32  K33  K34    K35  K36  K37
     14,  19,  24,   60,  55,  50,
};

// Toggle indicators, left half then right (see MIRYOKU_FREE_*).
static const uint8_t free_to_led[] = {
     4,      // MIRYOKU_FREE_CAPSLOCK    (left: bottom row, outer key)
     9,      // MIRYOKU_FREE_SCROLLLOCK  (left: bottom row, second key)
    NO_LED,  // MIRYOKU_FREE_JIGGLER     (right half only)
    40,      // MIRYOKU_FREE_CAPSLOCK    (right: bottom row, outer key)
    45,      // MIRYOKU_FREE_SCROLLLOCK  (right: bottom row, second key)
    63,      // MIRYOKU_FREE_JIGGLER     (right: spare key beside M)
};

// Agent slots down the left's spare column beside the index finger, top to
// bottom.
static const uint8_t herdr_slot_leds[HERDR_SLOT_COUNT] = {25, 26, 27, 28};

static const miryoku_leds_t leds = {
    .keys             = miryoku_to_led,
    .free_leds        = free_to_led,
    .free_led_count   = ARRAY_SIZE(free_to_led),
    .herdr_slots      = herdr_slot_leds,
    .herdr_connection = 64, // the right's spare key beside K
    .herdr_sort       = 65, // the right's innermost number-row key
};

bool rgb_matrix_indicators_user(void) { return miryoku_rgb_indicators(&leds); }
