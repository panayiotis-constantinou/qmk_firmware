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

extern bool capslock_active;
extern bool scrolllock_active;

// Shared helpers
RGB  hsv_to_rgb_with_value(HSV hsv);
bool miryoku_led_update_user(led_t led_state);
void set_layer_color_miryoku(int layer, const uint8_t *led_map);
void set_free_led_indicators(const uint8_t *free_leds, uint8_t count);

// ledmap[layer][miryoku_key_index][HSV]
// layers: 0=U_BASE, 4=U_NAV, 5=U_MOUSE, 6=U_MEDIA, 7=U_NUM, 8=U_SYM, 9=U_FUN
extern const uint8_t PROGMEM miryoku_ledmap[][MIRYOKU_KEY_COUNT][3];

#endif // RGB_MATRIX_ENABLE
