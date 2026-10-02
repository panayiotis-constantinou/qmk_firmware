// Copyright 2024 Manna Harbour
// https://github.com/manna-harbour/miryoku
// Shared RGB matrix definitions for manna-harbour_miryoku keyboards

#pragma once
#ifdef RGB_MATRIX_ENABLE

#include "quantum.h"
#include "mouse_jiggler.h"

// Number of active miryoku keys: K00-K09, K10-K19, K20-K29, K32-K34, K35-K37
#define MIRYOKU_KEY_COUNT 36

// Miryoku key indices (0-35):
//  0- 9: K00-K09  (top row,    left then right)
// 10-19: K10-K19  (home row,   left then right)
// 20-29: K20-K29  (bottom row, left then right)
// 30-32: K32-K34  (thumbs left)
// 33-35: K35-K37  (thumbs right)

// Indices into each keyboard's free_to_led[] array
#define MIRYOKU_FREE_CAPSLOCK    0
#define MIRYOKU_FREE_SCROLLLOCK  1
#define MIRYOKU_FREE_JIGGLER     2
#define MIRYOKU_FREE_LED_COUNT   3   // per half; split keyboards may double this

// Indicator HSV colors
#define MIRYOKU_HSV_CAPSLOCK    249, 239, 228
#define MIRYOKU_HSV_SCROLLLOCK  249, 218, 204
#define MIRYOKU_HSV_JIGGLER      85, 255, 220

// Catppuccin Mocha hues for the Herdr layer.  The Mocha accents are pastels
// that wash out to white on LEDs, so they share one boosted saturation.
#ifndef MIRYOKU_CTP_SAT
#    define MIRYOKU_CTP_SAT 200
#endif
#define CTP_RED      243
#define CTP_MAROON   248
#define CTP_PEACH     16
#define CTP_YELLOW    29
#define CTP_GREEN     82
#define CTP_TEAL     120
#define CTP_SAPPHIRE 141
#define CTP_BLUE     154
#define CTP_MAUVE    189
// Neutrals keep a low saturation of their own.
#define CTP_HSV_TEXT_HUE 160
#define CTP_HSV_TEXT_SAT  41
#define CTP_HSV_TEXT     CTP_HSV_TEXT_HUE, CTP_HSV_TEXT_SAT, 244
#define CTP_HSV_SURFACE  168, 120,  40
#define CTP_HSV_OFF      163,  90,  32

// Keypress inactivity after which only Herdr slots needing attention stay lit.
// Replaces RGB_MATRIX_TIMEOUT, which also hides the indicators.
#ifndef MIRYOKU_LED_IDLE_TIMEOUT
#    define MIRYOKU_LED_IDLE_TIMEOUT 600000
#endif

// Shared helpers
RGB  hsv_to_rgb_with_value(HSV hsv);
bool miryoku_leds_idle(void);
bool miryoku_layer_has_colors(uint8_t layer);
void set_layer_color_miryoku(uint8_t layer, const uint8_t *led_map);
void set_agent_layer_colors_miryoku(const uint8_t *led_map);
void set_free_led_indicators(const uint8_t *free_leds, uint8_t count);

// Where a ZSA keyboard's LEDs sit, for miryoku_rgb_indicators().
typedef struct {
    const uint8_t *keys;             // Miryoku key index -> LED (MIRYOKU_KEY_COUNT)
    const uint8_t *free_leds;        // toggle indicators, see MIRYOKU_FREE_*
    uint8_t        free_led_count;
    const uint8_t *led_only;         // positions without a switch, kept dark
    uint8_t        led_only_count;
    const uint8_t *herdr_slots;      // HERDR_SLOT_COUNT agent slots
    uint8_t        herdr_connection; // host connection LED
    uint8_t        herdr_sort;       // panel sort mode LED
} miryoku_leds_t;

// Draws the layer colors, toggle indicators, and Herdr status; a keymap's
// rgb_matrix_indicators_user returns it.
bool miryoku_rgb_indicators(const miryoku_leds_t *leds);

// ledmap[layer][miryoku_key_index][HSV], for layers miryoku_layer_has_colors
// accepts.
extern const uint8_t PROGMEM miryoku_ledmap[][MIRYOKU_KEY_COUNT][3];

#endif // RGB_MATRIX_ENABLE
