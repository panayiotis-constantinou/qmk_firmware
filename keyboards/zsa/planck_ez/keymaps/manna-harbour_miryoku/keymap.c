#include QMK_KEYBOARD_H
#include "manna-harbour_miryoku.h"
#include "miryoku_rgb.h"

// miryoku key index -> planck LED index (36 entries)
// index 0-9=K00-K09, 10-19=K10-K19, 20-29=K20-K29, 30-32=K32-K34, 33-35=K35-K37
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

// Free (non-miryoku) LED index -> planck LED index
// Inner key per row: row0=5, row1=17, row2=29
static const uint8_t free_to_led[] = {
     5,  // MIRYOKU_FREE_CAPSLOCK
    17,  // MIRYOKU_FREE_SCROLLLOCK
    29,  // MIRYOKU_FREE_JIGGLER
};

bool led_update_user(led_t s) { return miryoku_led_update_user(s); }

bool rgb_matrix_indicators_user(void) {
    if (!keyboard_config.disable_layer_led) {
        int layer = biton32(layer_state);
        if (layer == 0 || (layer >= 4 && layer <= 9)) {
            set_layer_color_miryoku(layer, miryoku_to_led);
        } else if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
            rgb_matrix_set_color_all(0, 0, 0);
        }
    } else if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
        rgb_matrix_set_color_all(0, 0, 0);
    }
    set_free_led_indicators(free_to_led, sizeof(free_to_led));
    return true;
}
