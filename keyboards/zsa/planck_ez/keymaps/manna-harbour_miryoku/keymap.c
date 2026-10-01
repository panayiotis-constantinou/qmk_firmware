#include QMK_KEYBOARD_H
#include "manna-harbour_miryoku.h"
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
// Agent slots on the center block's top two rows, left to right, and the host
// connection on the space bar.
static const uint8_t herdr_agent_leds[HERDR_SLOT_COUNT] = {5, 6, 17, 18};
#define HERDR_CONNECTION_LED 41

static void render_herdr(void) {
    for (uint8_t i = 0; i < ARRAY_SIZE(led_only_leds); ++i) {
        rgb_matrix_set_color(led_only_leds[i], 0, 0, 0);
    }

    if (layer_state_is(U_AGENT)) {
        set_agent_layer_colors_miryoku(miryoku_to_led);
    }

    set_free_led_indicators(free_to_led, ARRAY_SIZE(free_to_led));

    herdr_render_status(HERDR_CONNECTION_LED, herdr_agent_leds);
}

bool led_update_user(led_t state) { return miryoku_led_update_user(state); }

bool rgb_matrix_indicators_user(void) {
    if (miryoku_leds_idle()) {
        rgb_matrix_set_color_all(0, 0, 0);
        herdr_render_status(HERDR_CONNECTION_LED, herdr_agent_leds);
        return true;
    }
    if (!keyboard_config.disable_layer_led && rgb_matrix_get_mode() == RGB_MATRIX_SOLID_COLOR) {
        int layer = biton32(layer_state);
        if (layer == 0 || (layer >= 4 && layer <= 9)) {
            set_layer_color_miryoku(layer, miryoku_to_led);
        } else if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
            rgb_matrix_set_color_all(0, 0, 0);
        }
    } else if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
        rgb_matrix_set_color_all(0, 0, 0);
    }
    render_herdr();
    return true;
}
