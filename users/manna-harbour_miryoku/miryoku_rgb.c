// Copyright 2024 Manna Harbour
// https://github.com/manna-harbour/miryoku
// Shared RGB matrix implementation for manna-harbour_miryoku keyboards

#ifdef RGB_MATRIX_ENABLE

#include "quantum.h"
#include "manna-harbour_miryoku.h"
#include "miryoku_rgb.h"
#include "herdr.h"

extern rgb_config_t rgb_matrix_config;

// ── Capslock / Scrolllock state ──────────────────────────────────────────────

bool capslock_active   = false;
bool scrolllock_active = false;

bool miryoku_led_update_user(led_t led_state) {
    capslock_active   = led_state.caps_lock;
    scrolllock_active = led_state.scroll_lock;
    return true;
}

bool miryoku_leds_idle(void) {
    return last_input_activity_elapsed() > MIRYOKU_LED_IDLE_TIMEOUT;
}

// ── HSV → RGB scaled by current brightness ──────────────────────────────────

RGB hsv_to_rgb_with_value(HSV hsv) {
    RGB   rgb = hsv_to_rgb(hsv);
    float f   = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
    return (RGB){f * rgb.r, f * rgb.g, f * rgb.b};
}

// ── Per-layer color setter ───────────────────────────────────────────────────

void set_layer_color_miryoku(int layer, const uint8_t *led_map) {
    rgb_matrix_set_color_all(0, 0, 0);
    for (int i = 0; i < MIRYOKU_KEY_COUNT; i++) {
        HSV hsv = {
            .h = pgm_read_byte(&miryoku_ledmap[layer][i][0]),
            .s = pgm_read_byte(&miryoku_ledmap[layer][i][1]),
            .v = pgm_read_byte(&miryoku_ledmap[layer][i][2]),
        };
        if (!hsv.h && !hsv.s && !hsv.v) {
            rgb_matrix_set_color(led_map[i], 0, 0, 0);
        } else {
            RGB rgb = hsv_to_rgb_with_value(hsv);
            rgb_matrix_set_color(led_map[i], rgb.r, rgb.g, rgb.b);
        }
    }
}

#ifdef HERDR_AGENT_ENABLE
// Accept shows the approval risk of the focused blocked agent.  A pending or
// failed judgement never looks green.
static HSV agent_accept_hsv(void) {
    switch (herdr_accept_risk()) {
        case HERDR_RISK_PENDING: {
            uint16_t phase = (timer_read32() / 4) % 512;
            uint8_t  level = phase < 256 ? phase : 511 - phase;
            return (HSV){CTP_HSV_TEXT_HUE, CTP_HSV_TEXT_SAT, 60 + level * 160 / 255};
        }
        case HERDR_RISK_UNKNOWN:
            return (HSV){CTP_HSV_TEXT};
        case HERDR_RISK_MEDIUM:
            return (HSV){CTP_PEACH, MIRYOKU_CTP_SAT, 180};
        case HERDR_RISK_HIGH:
            return (HSV){CTP_RED, MIRYOKU_CTP_SAT, 180};
        default:
            return (HSV){CTP_GREEN, MIRYOKU_CTP_SAT, 180};
    }
}

void set_agent_layer_colors_miryoku(const uint8_t *led_map) {
    RGB rgb = hsv_to_rgb_with_value((HSV){CTP_HSV_SURFACE});
    for (int i = 0; i < MIRYOKU_KEY_COUNT; i++) {
        rgb_matrix_set_color(led_map[i], rgb.r, rgb.g, rgb.b);
    }

#define SET_AGENT_KEY(index, hue) do { \
    rgb = hsv_to_rgb_with_value((HSV){hue, MIRYOKU_CTP_SAT, 180}); \
    rgb_matrix_set_color(led_map[index], rgb.r, rgb.g, rgb.b); \
} while (0)
    SET_AGENT_KEY( 0, herdr_sort_is_grouped() ? CTP_PEACH : CTP_GREEN); // Herdr sort mode
    SET_AGENT_KEY( 6, CTP_GREEN);    // new workspace
    SET_AGENT_KEY( 7, CTP_GREEN);    // new tab
    SET_AGENT_KEY(11, CTP_PEACH);    // clear
    SET_AGENT_KEY(12, CTP_RED);      // reject
    SET_AGENT_KEY(13, CTP_BLUE);     // prompt
    SET_AGENT_KEY(15, CTP_MAUVE);    // agent picker
    SET_AGENT_KEY(16, CTP_YELLOW);   // pane left
    SET_AGENT_KEY(17, CTP_YELLOW);   // pane down
    SET_AGENT_KEY(18, CTP_YELLOW);   // pane up
    SET_AGENT_KEY(19, CTP_YELLOW);   // pane right
    SET_AGENT_KEY(20, CTP_TEAL);     // scratchpad
    SET_AGENT_KEY(21, CTP_PEACH);    // hunk diff
    SET_AGENT_KEY(22, CTP_PEACH);    // lazygit
    SET_AGENT_KEY(23, CTP_MAUVE);    // command palette
    SET_AGENT_KEY(25, CTP_YELLOW);   // pane zoom
    SET_AGENT_KEY(26, CTP_MAUVE);    // previous workspace
    SET_AGENT_KEY(27, CTP_SAPPHIRE); // previous tab
    SET_AGENT_KEY(28, CTP_SAPPHIRE); // next tab
    SET_AGENT_KEY(29, CTP_MAUVE);    // next workspace
    SET_AGENT_KEY(30, CTP_RED);      // escape
    SET_AGENT_KEY(31, CTP_MAUVE);    // previous agent
    SET_AGENT_KEY(32, CTP_MAUVE);    // next agent
    SET_AGENT_KEY(34, CTP_MAUVE);    // most urgent agent
    SET_AGENT_KEY(35, CTP_MAROON);   // delete
#undef SET_AGENT_KEY

    rgb = hsv_to_rgb_with_value(agent_accept_hsv());
    rgb_matrix_set_color(led_map[33], rgb.r, rgb.g, rgb.b);

    // Keys waiting for their confirming tap light Catppuccin text.
    static const uint16_t cued_keys[][2] = {
        {AG_ACCEPT, 33}, {AG_REJECT, 12}, {AG_CLEAR, 11},
    };
    rgb = hsv_to_rgb_with_value((HSV){CTP_HSV_TEXT});
    for (uint8_t i = 0; i < ARRAY_SIZE(cued_keys); ++i) {
        if (herdr_is_armed(cued_keys[i][0])) {
            rgb_matrix_set_color(led_map[cued_keys[i][1]], rgb.r, rgb.g, rgb.b);
        }
    }

    rgb = hsv_to_rgb_with_value(herdr_sounds_are_enabled() ?
        (HSV){CTP_GREEN, MIRYOKU_CTP_SAT, 180} : (HSV){CTP_HSV_OFF});
    rgb_matrix_set_color(led_map[9], rgb.r, rgb.g, rgb.b);
}
#endif

// ── Free-LED toggle indicators ───────────────────────────────────────────────

void set_free_led_indicators(const uint8_t *free_leds, uint8_t count) {
    if (capslock_active) {
        RGB rgb = hsv_to_rgb_with_value((HSV){MIRYOKU_HSV_CAPSLOCK});
        for (int i = MIRYOKU_FREE_CAPSLOCK; i < count; i += MIRYOKU_FREE_LED_COUNT)
            rgb_matrix_set_color(free_leds[i], rgb.r, rgb.g, rgb.b);
    }
    if (scrolllock_active) {
        RGB rgb = hsv_to_rgb_with_value((HSV){MIRYOKU_HSV_SCROLLLOCK});
        for (int i = MIRYOKU_FREE_SCROLLLOCK; i < count; i += MIRYOKU_FREE_LED_COUNT)
            rgb_matrix_set_color(free_leds[i], rgb.r, rgb.g, rgb.b);
    }
    if (mouse_jiggler_is_enabled()) {
        RGB rgb = hsv_to_rgb_with_value((HSV){MIRYOKU_HSV_JIGGLER});
        for (int i = MIRYOKU_FREE_JIGGLER; i < count; i += MIRYOKU_FREE_LED_COUNT)
            rgb_matrix_set_color(free_leds[i], rgb.r, rgb.g, rgb.b);
    }
}

// ── Ledmap: colors keyed by miryoku key index ────────────────────────────────
// Sourced from the moonlander keymap's per-LED ledmap, remapped through
// the moonlander's miryoku_to_led[] array so colors are keyboard-agnostic.

const uint8_t PROGMEM miryoku_ledmap[][MIRYOKU_KEY_COUNT][3] = {
    [0] = { // U_BASE
        /* K00-K04 */ {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241},
        /* K05-K09 */ {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {86,161,164},
        /* K10-K14 */ {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241},
        /* K15-K19 */ {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241},
        /* K20-K24 */ {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241},
        /* K25-K29 */ {167,253,241}, {167,253,241}, {86,161,164}, {86,161,164}, {86,161,164},
        /* K32-K34 */ {231,168,159}, {81,61,255}, {167,94,200},
        /* K35-K37 */ {41,126,255}, {219,73,176}, {251,208,228}
    },
    [4] = { // U_NAV
        /* K00-K04 */ {255,218,204}, {23,236,165}, {23,236,165}, {94,172,234}, {0,0,0},
        /* K05-K09 */ {0,0,0}, {171,244,228}, {171,244,228}, {171,244,228}, {0,0,0},
        /* K10-K14 */ {154,81,210}, {154,81,210}, {154,81,210}, {154,81,210}, {0,0,0},
        /* K15-K19 */ {94,172,234}, {29,116,255}, {29,116,255}, {29,116,255}, {29,116,255},
        /* K20-K24 */ {0,0,0}, {154,81,210}, {0,0,0}, {0,0,0}, {0,0,0},
        /* K25-K29 */ {94,172,234}, {154,81,210}, {154,81,210}, {154,81,210}, {154,81,210},
        /* K32-K34 */ {0,0,0}, {0,0,0}, {0,0,0},
        /* K35-K37 */ {41,126,255}, {190,116,238}, {0,163,255}
    },
    [5] = { // U_MOUSE
        /* K00-K04 */ {255,218,204}, {87,218,204}, {87,218,204}, {87,218,204}, {0,0,0},
        /* K05-K09 */ {87,218,204}, {87,218,204}, {170,218,204}, {87,218,204}, {87,218,204},
        /* K10-K14 */ {154,81,210}, {154,81,210}, {154,81,210}, {154,81,210}, {0,0,0},
        /* K15-K19 */ {0,0,0}, {170,218,204}, {170,218,204}, {170,218,204}, {87,218,204},
        /* K20-K24 */ {0,0,0}, {154,81,210}, {0,0,0}, {0,0,0}, {0,0,0},
        /* K25-K29 */ {167,253,241}, {190,114,193}, {190,114,193}, {190,114,193}, {190,114,193},
        /* K32-K34 */ {250,223,147}, {250,223,147}, {0,0,0},
        /* K35-K37 */ {50,191,255}, {50,191,255}, {50,191,255}
    },
    [6] = { // U_MEDIA
        /* K00-K04 */ {255,218,204}, {25,217,217}, {25,217,217}, {0,0,0}, {0,0,0},
        /* K05-K09 */ {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
        /* K10-K14 */ {154,81,210}, {154,81,210}, {154,81,210}, {154,81,210}, {0,0,0},
        /* K15-K19 */ {0,0,0}, {25,84,255}, {25,84,255}, {25,84,255}, {25,84,255},
        /* K20-K24 */ {0,0,0}, {154,81,210}, {0,0,0}, {0,0,0}, {0,0,0},
        /* K25-K29 */ {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
        /* K32-K34 */ {0,0,0}, {0,0,0}, {0,0,0},
        /* K35-K37 */ {25,84,255}, {25,84,255}, {25,84,255}
    },
    [7] = { // U_NUM
        /* K00-K04 */ {0,0,0}, {234,199,193}, {234,199,193}, {234,199,193}, {0,0,0},
        /* K05-K09 */ {255,218,204}, {43,189,217}, {43,189,217}, {43,189,217}, {43,189,217},
        /* K10-K14 */ {0,0,0}, {234,199,193}, {234,199,193}, {234,199,193}, {139,136,177},
        /* K15-K19 */ {0,0,0}, {154,81,210}, {154,81,210}, {154,81,210}, {154,81,210},
        /* K20-K24 */ {0,0,0}, {234,199,193}, {234,199,193}, {234,199,193}, {0,0,0},
        /* K25-K29 */ {0,0,0}, {0,0,0}, {0,0,0}, {154,81,210}, {0,0,0},
        /* K32-K34 */ {0,0,0}, {234,199,193}, {0,0,0},
        /* K35-K37 */ {0,0,0}, {0,0,0}, {0,0,0}
    },
    [8] = { // U_SYM
        /* K00-K04 */ {250,215,188}, {0,0,0}, {0,0,0}, {95,132,234}, {250,215,188},
        /* K05-K09 */ {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,218,204},
        /* K10-K14 */ {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
        /* K15-K19 */ {0,0,0}, {154,81,210}, {154,81,210}, {154,81,210}, {154,81,210},
        /* K20-K24 */ {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
        /* K25-K29 */ {0,0,0}, {0,0,0}, {0,0,0}, {154,81,210}, {0,0,0},
        /* K32-K34 */ {195,151,136}, {95,132,234}, {0,0,0},
        /* K35-K37 */ {0,0,0}, {195,151,136}, {195,151,136}
    },
    [9] = { // U_FUN
        /* K00-K04 */ {41,196,251}, {41,196,251}, {41,196,251}, {41,196,251}, {0,0,0},
        /* K05-K09 */ {0,0,0}, {190,160,159}, {190,160,159}, {190,160,159}, {255,218,204},
        /* K10-K14 */ {41,196,251}, {41,196,251}, {41,196,251}, {41,196,251}, {91,228,49},
        /* K15-K19 */ {0,0,0}, {154,81,210}, {154,81,210}, {154,81,210}, {154,81,210},
        /* K20-K24 */ {41,196,251}, {41,196,251}, {41,196,251}, {41,196,251}, {0,0,0},
        /* K25-K29 */ {0,0,0}, {0,0,0}, {154,81,210}, {0,0,0}, {0,0,0},
        /* K32-K34 */ {0,0,0}, {81,61,255}, {131,128,238},
        /* K35-K37 */ {0,0,0}, {0,0,0}, {0,0,0}
    }
};

#endif // RGB_MATRIX_ENABLE
