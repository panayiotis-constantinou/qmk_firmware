#include QMK_KEYBOARD_H
#include "version.h"
#include "i18n.h"
#include "os_detection.h"
#define MOON_LED_LEVEL LED_LEVEL
#ifndef ZSA_SAFE_RANGE
#define ZSA_SAFE_RANGE SAFE_RANGE
#endif

enum custom_keycodes {
  RGB_SLD = ZSA_SAFE_RANGE,
  MAC_MISSION_CONTROL,
  OSMODE_TOGG,
  KC_MS_JIGGLER_TOGGLE,
};

typedef enum { OS_AUTO = 0, OS_FORCE_MAC = 1, OS_FORCE_LINUX = 2 } os_override_t;

static os_variant_t  detected_os = OS_UNSURE;
static os_override_t os_override  = OS_AUTO;

static bool     jiggler_active = false;
static uint32_t jiggler_timer  = 0;
static bool     jiggler_confirm_active = false;
static uint8_t  jiggler_confirm_step   = 0;
static uint32_t jiggler_confirm_timer  = 0;

enum {
    JIGGLER_INTERVAL_MS      = 2000,
    JIGGLER_STEP             = 10,
    JIGGLER_CONFIRM_STEP     = 20,
    JIGGLER_CONFIRM_DELAY_MS = 120,
    JIGGLER_CONFIRM_STEPS    = 4,
};

static const int8_t jiggler_confirm_dx[JIGGLER_CONFIRM_STEPS] = {JIGGLER_CONFIRM_STEP, -JIGGLER_CONFIRM_STEP, JIGGLER_CONFIRM_STEP, -JIGGLER_CONFIRM_STEP};

static void send_mouse_delta(int8_t x, int8_t y) {
    report_mouse_t report = {0};
    report.x = x;
    report.y = y;
    host_mouse_send(&report);
}

static bool is_linux_mode(void) {
    if (os_override == OS_FORCE_LINUX) return true;
    if (os_override == OS_FORCE_MAC)   return false;
    return (detected_os == OS_LINUX || detected_os == OS_WINDOWS);
}

bool process_detected_host_os_user(os_variant_t detected) {
    detected_os = detected;
    return true;
}

void eeconfig_init_user(void) {
    eeconfig_update_user(OS_AUTO);
}



enum tap_dance_codes {
  DANCE_0,
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_Q,           KC_W,           KC_F,           KC_P,           KC_B,           KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_J,           KC_L,           KC_U,           KC_Y,           KC_QUOTE,       
    MT(MOD_LGUI, KC_A),MT(MOD_LALT, KC_R),MT(MOD_LCTL, KC_S),MT(MOD_LSFT, KC_T),KC_G,           KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_M,           MT(MOD_LSFT, KC_N),MT(MOD_LCTL, KC_E),MT(MOD_LALT, KC_I),MT(MOD_LGUI, KC_O),
    KC_Z,           MT(MOD_RALT, KC_X),KC_C,           KC_D,           KC_V,           KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_K,           KC_H,           KC_COMMA,       MT(MOD_RALT, KC_DOT),KC_SLASH,       
    KC_TRANSPARENT, KC_TRANSPARENT, LT(3, KC_ESCAPE),LT(1, KC_SPACE),LT(2, KC_TAB),  KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, LT(5, KC_ENTER),TD(DANCE_0),    LT(6, KC_DELETE),KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [1] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    QK_BOOT,        KC_MAC_UNDO,    KC_NO,          CW_TOGG,        KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_MAC_PASTE,   KC_MAC_COPY,    KC_MAC_CUT,     KC_NO,          
    KC_LEFT_GUI,    KC_LEFT_ALT,    KC_LEFT_CTRL,   KC_LEFT_SHIFT,  KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_CAPS,        KC_LEFT,        KC_DOWN,        KC_UP,          KC_RIGHT,       
    KC_NO,          KC_RIGHT_ALT,   KC_NO,          KC_NO,          KC_NO,          KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_INSERT,      KC_HOME,        KC_PGDN,        KC_PAGE_UP,     KC_END,         
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_BSPC,        KC_DELETE,      KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_NO,          KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [2] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    QK_BOOT,        KC_MAC_CUT,     KC_MAC_COPY,    KC_MAC_PASTE,   KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, LSFT(KC_INSERT),KC_NO,          MS_UP,       KC_PC_UNDO,     LSFT(KC_DELETE),
    KC_LEFT_GUI,    KC_LEFT_ALT,    KC_LEFT_CTRL,   KC_LEFT_SHIFT,  KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          MS_LEFT,     MS_DOWN,     MS_RGHT,    LCTL(KC_INSERT),
    MAC_MISSION_CONTROL,KC_RIGHT_ALT,   KC_NO,          KC_NO,          KC_NO,          KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_MS_JIGGLER_TOGGLE,MS_WHLL,  MS_WHLD,  MS_WHLU,    MS_WHLR,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, MS_BTN2,     MS_BTN1,     MS_BTN3,     KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [3] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    QK_BOOT,        RM_VALD,        RM_VALU,        KC_NO,          KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, RM_TOGG,        RM_NEXT,RM_HUEU,        RM_SATU,        RM_VALU,        
    KC_LEFT_GUI,    KC_LEFT_ALT,    KC_LEFT_CTRL,   KC_LEFT_SHIFT,  OSMODE_TOGG,        KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_MEDIA_PREV_TRACK,KC_AUDIO_VOL_DOWN,KC_AUDIO_VOL_UP,KC_MEDIA_NEXT_TRACK,
    KC_NO,          KC_RIGHT_ALT,   KC_NO,          KC_NO,          KC_NO,          KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_MEDIA_STOP,  KC_MEDIA_PLAY_PAUSE,KC_AUDIO_MUTE,  KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [4] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_LBRC,        KC_7,           KC_8,           KC_9,           KC_RBRC,        KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, QK_BOOT,        KC_PLUS,        KC_MINUS,       KC_ASTR,        KC_SLASH,       
    KC_SCLN,        KC_4,           KC_5,           KC_6,           KC_EQUAL,       KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_LEFT_SHIFT,  KC_LEFT_CTRL,   KC_LEFT_ALT,    KC_LEFT_GUI,    
    KC_GRAVE,       KC_1,           KC_2,           KC_3,           KC_BSLS,        KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_NO,          KC_NO,          KC_NO,          KC_RIGHT_ALT,   KC_NO,          
    KC_TRANSPARENT, KC_TRANSPARENT, KC_DOT,         KC_0,           KC_MINUS,       KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [5] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_LCBR,        KC_AMPR,        KC_ASTR,        KC_LPRN,        KC_RCBR,        KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_NO,          KC_NO,          KC_NO,          QK_BOOT,        
    KC_COLN,        KC_DLR,         KC_PERC,        KC_CIRC,        KC_PLUS,        KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_LEFT_SHIFT,  KC_LEFT_CTRL,   KC_LEFT_ALT,    KC_LEFT_GUI,    
    KC_TILD,        KC_EXLM,        KC_AT,          KC_HASH,        KC_PIPE,        KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_NO,          KC_NO,          KC_NO,          KC_RIGHT_ALT,   KC_NO,          
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_RPRN,        KC_UNDS,        KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [6] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_F12,         KC_F7,          KC_F8,          KC_F9,          KC_PSCR,        KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, QK_BOOT,        QK_DYNAMIC_TAPPING_TERM_PRINT,QK_DYNAMIC_TAPPING_TERM_DOWN,QK_DYNAMIC_TAPPING_TERM_UP,KC_NO,          
    KC_F11,         KC_F4,          KC_F5,          KC_F6,          KC_SCRL,        KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_LEFT_SHIFT,  KC_LEFT_CTRL,   KC_LEFT_ALT,    KC_LEFT_GUI,    
    KC_F10,         KC_F1,          KC_F2,          KC_F3,          KC_PAUSE,       KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_NO,          KC_NO,          KC_RIGHT_ALT,   KC_NO,          KC_NO,          
    KC_TRANSPARENT, KC_TRANSPARENT, KC_APPLICATION, KC_SPACE,       KC_TAB,         KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
};




bool capslock_active = false;
bool scrolllock_active = false;

bool led_update_user(led_t led_state) {
  capslock_active = led_state.caps_lock;
  scrolllock_active = led_state.scroll_lock;
  return true;
}

extern rgb_config_t rgb_matrix_config;

RGB hsv_to_rgb_with_value(HSV hsv) {
  RGB rgb = hsv_to_rgb( hsv );
  float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
  return (RGB){ f * rgb.r, f * rgb.g, f * rgb.b };
}

void keyboard_post_init_user(void) {
  rgb_matrix_enable();
  os_override = (os_override_t)(eeconfig_read_user() & 0x03);
}

const uint8_t PROGMEM ledmap[][RGB_MATRIX_LED_COUNT][3] = {
    [0] = { {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {231,168,159}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {81,61,255}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,94,200}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {0,0,0}, {0,0,0}, {0,0,0}, {171,239,96}, {167,253,241}, {86,161,164}, {167,253,241}, {86,161,164}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {86,161,164}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {86,161,164}, {251,208,228}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {219,73,176}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {41,126,255}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {167,253,241}, {0,0,0}, {0,0,0}, {0,0,0}, {171,239,96} },

    [1] = { {203,240,200}, {255,218,204}, {154,81,210}, {0,0,0}, {203,240,200}, {203,240,200}, {23,236,165}, {154,81,210}, {154,81,210}, {203,240,200}, {203,240,200}, {23,236,165}, {154,81,210}, {0,0,0}, {91,228,49}, {203,240,200}, {94,172,234}, {154,81,210}, {0,0,0}, {91,228,49}, {203,240,200}, {0,0,0}, {0,0,0}, {0,0,0}, {92,190,153}, {203,240,200}, {203,240,200}, {203,240,200}, {203,240,200}, {203,240,200}, {203,240,200}, {203,240,200}, {0,0,0}, {0,0,0}, {91,228,49}, {91,228,49}, {203,240,200}, {0,0,0}, {29,116,255}, {154,81,210}, {203,240,200}, {203,240,200}, {171,244,228}, {29,116,255}, {154,81,210}, {203,240,200}, {203,240,200}, {171,244,228}, {29,116,255}, {154,81,210}, {0,163,255}, {203,240,200}, {171,244,228}, {29,116,255}, {154,81,210}, {190,116,238}, {203,240,200}, {0,0,0}, {94,172,234}, {94,172,234}, {91,228,49}, {203,240,200}, {203,240,200}, {203,240,200}, {203,240,200}, {203,240,200}, {203,240,200}, {203,240,200}, {91,228,49}, {145,57,250}, {91,228,49}, {91,228,49} },

    [2] = { {250,223,147}, {255,218,204}, {154,81,210}, {0,0,0}, {250,223,147}, {250,223,147}, {87,218,204}, {154,81,210}, {154,81,210}, {250,223,147}, {250,223,147}, {87,218,204}, {154,81,210}, {0,0,0}, {250,223,147}, {250,223,147}, {87,218,204}, {154,81,210}, {0,0,0}, {250,223,147}, {250,223,147}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {250,223,147}, {250,223,147}, {250,223,147}, {250,223,147}, {250,223,147}, {250,223,147}, {250,223,147}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {250,223,147}, {87,218,204}, {87,218,204}, {190,114,193}, {250,223,147}, {250,223,147}, {87,218,204}, {170,218,204}, {190,114,193}, {250,223,147}, {250,223,147}, {170,218,204}, {170,218,204}, {190,114,193}, {50,191,255}, {250,223,147}, {87,218,204}, {170,218,204}, {190,114,193}, {50,191,255}, {250,223,147}, {87,218,204}, {0,0,0}, {167,253,241}, {50,191,255}, {250,223,147}, {250,223,147}, {250,223,147}, {250,223,147}, {250,223,147}, {250,223,147}, {250,223,147}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} },

    [3] = { {147,243,206}, {255,218,204}, {154,81,210}, {0,0,0}, {147,243,206}, {147,243,206}, {25,217,217}, {154,81,210}, {154,81,210}, {147,243,206}, {147,243,206}, {25,217,217}, {154,81,210}, {0,0,0}, {0,0,0}, {147,243,206}, {0,0,0}, {154,81,210}, {0,0,0}, {171,244,228}, {147,243,206}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {147,243,206}, {147,243,206}, {147,243,206}, {147,243,206}, {147,243,206}, {147,243,206}, {147,243,206}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {147,243,206}, {47,232,212}, {25,84,255}, {0,0,0}, {147,243,206}, {147,243,206}, {47,232,212}, {25,84,255}, {0,0,0}, {147,243,206}, {147,243,206}, {47,232,212}, {25,84,255}, {0,0,0}, {25,84,255}, {147,243,206}, {47,232,212}, {25,84,255}, {0,0,0}, {25,84,255}, {147,243,206}, {47,232,212}, {0,0,0}, {0,0,0}, {25,84,255}, {147,243,206}, {147,243,206}, {147,243,206}, {147,243,206}, {147,243,206}, {147,243,206}, {147,243,206}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} },

    [4] = { {0,240,199}, {0,0,0}, {0,0,0}, {0,0,0}, {0,240,199}, {0,240,199}, {234,199,193}, {234,199,193}, {234,199,193}, {0,240,199}, {0,240,199}, {234,199,193}, {234,199,193}, {234,199,193}, {0,0,0}, {0,240,199}, {234,199,193}, {234,199,193}, {234,199,193}, {234,199,193}, {0,240,199}, {0,0,0}, {139,136,177}, {0,0,0}, {0,0,0}, {0,240,199}, {0,240,199}, {0,240,199}, {0,240,199}, {0,240,199}, {0,240,199}, {0,240,199}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,240,199}, {43,189,217}, {154,81,210}, {0,0,0}, {0,240,199}, {0,240,199}, {43,189,217}, {154,81,210}, {154,81,210}, {0,240,199}, {0,240,199}, {43,189,217}, {154,81,210}, {0,0,0}, {0,0,0}, {0,240,199}, {43,189,217}, {154,81,210}, {0,0,0}, {0,0,0}, {0,240,199}, {255,218,204}, {0,0,0}, {0,0,0}, {0,240,199}, {0,240,199}, {0,240,199}, {0,240,199}, {0,240,199}, {0,240,199}, {0,240,199}, {0,240,199}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} },

    [5] = { {195,151,136}, {250,215,188}, {0,0,0}, {0,0,0}, {195,151,136}, {195,151,136}, {0,0,0}, {0,0,0}, {0,0,0}, {195,151,136}, {195,151,136}, {0,0,0}, {0,0,0}, {0,0,0}, {195,151,136}, {195,151,136}, {95,132,234}, {0,0,0}, {0,0,0}, {95,132,234}, {195,151,136}, {250,215,188}, {0,0,0}, {0,0,0}, {0,0,0}, {195,151,136}, {195,151,136}, {195,151,136}, {195,151,136}, {195,151,136}, {195,151,136}, {195,151,136}, {195,151,136}, {0,0,0}, {195,151,136}, {195,151,136}, {195,151,136}, {255,218,204}, {154,81,210}, {0,0,0}, {195,151,136}, {195,151,136}, {0,0,0}, {154,81,210}, {154,81,210}, {195,151,136}, {195,151,136}, {0,0,0}, {154,81,210}, {0,0,0}, {195,151,136}, {195,151,136}, {0,0,0}, {154,81,210}, {0,0,0}, {195,151,136}, {195,151,136}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {195,151,136}, {195,151,136}, {195,151,136}, {195,151,136}, {195,151,136}, {195,151,136}, {195,151,136}, {0,0,0}, {0,0,0}, {195,151,136}, {195,151,136} },

    [6] = { {25,217,217}, {41,196,251}, {41,196,251}, {41,196,251}, {25,217,217}, {25,217,217}, {41,196,251}, {41,196,251}, {41,196,251}, {25,217,217}, {25,217,217}, {41,196,251}, {41,196,251}, {41,196,251}, {0,0,0}, {25,217,217}, {41,196,251}, {41,196,251}, {41,196,251}, {81,61,255}, {25,217,217}, {0,0,0}, {91,228,49}, {0,0,0}, {131,128,238}, {25,217,217}, {25,217,217}, {25,217,217}, {25,217,217}, {25,217,217}, {25,217,217}, {25,217,217}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {25,217,217}, {0,0,0}, {154,81,210}, {0,0,0}, {25,217,217}, {25,217,217}, {190,160,159}, {154,81,210}, {0,0,0}, {25,217,217}, {25,217,217}, {190,160,159}, {154,81,210}, {154,81,210}, {0,0,0}, {25,217,217}, {190,160,159}, {154,81,210}, {0,0,0}, {0,0,0}, {25,217,217}, {255,218,204}, {0,0,0}, {0,0,0}, {0,0,0}, {25,217,217}, {25,217,217}, {25,217,217}, {25,217,217}, {25,217,217}, {25,217,217}, {25,217,217}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} },

};

void set_layer_color(int layer) {
  for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
    HSV hsv = {
      .h = pgm_read_byte(&ledmap[layer][i][0]),
      .s = pgm_read_byte(&ledmap[layer][i][1]),
      .v = pgm_read_byte(&ledmap[layer][i][2]),
    };
    if (!hsv.h && !hsv.s && !hsv.v) {
        rgb_matrix_set_color( i, 0, 0, 0 );
    } else {
        RGB rgb = hsv_to_rgb_with_value(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
  }
}

bool rgb_matrix_indicators_user(void) {
  if (!keyboard_config.disable_layer_led) { 
    switch (biton32(layer_state)) {
      case 0:
        set_layer_color(0);
        break;
      case 1:
        set_layer_color(1);
        break;
      case 2:
        set_layer_color(2);
        break;
      case 3:
        set_layer_color(3);
        break;
      case 4:
        set_layer_color(4);
        break;
      case 5:
        set_layer_color(5);
        break;
      case 6:
        set_layer_color(6);
        break;
     default:
        if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
          rgb_matrix_set_color_all(0, 0, 0);
        }
    }
  } else {
    if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
      rgb_matrix_set_color_all(0, 0, 0);
    }
  }

  if (capslock_active && biton32(layer_state) == 1) {
    RGB rgb = hsv_to_rgb_with_value((HSV) { 249, 239, 228 });
    rgb_matrix_set_color( 58, rgb.r, rgb.g, rgb.b );
  } 
  if (scrolllock_active && biton32(layer_state) == 6) {
    RGB rgb = hsv_to_rgb_with_value((HSV) { 249, 218, 204 });
    rgb_matrix_set_color( 22, rgb.r, rgb.g, rgb.b );
  }
  if (jiggler_active) {
    RGB rgb = hsv_to_rgb_with_value((HSV){ 85, 255, 220 });
    rgb_matrix_set_color(59, rgb.r, rgb.g, rgb.b);
  }
  return true;
}

void housekeeping_task_user(void) {
    if (jiggler_confirm_active && !jiggler_active) {
        jiggler_confirm_active = false;
    }

    if (jiggler_confirm_active && timer_elapsed32(jiggler_confirm_timer) >= JIGGLER_CONFIRM_DELAY_MS) {
        send_mouse_delta(jiggler_confirm_dx[jiggler_confirm_step], 0);
        jiggler_confirm_step++;
        jiggler_confirm_timer = timer_read32();
        if (jiggler_confirm_step >= JIGGLER_CONFIRM_STEPS) {
            jiggler_confirm_active = false;
        }
    }

    if (jiggler_active && timer_elapsed32(jiggler_timer) > JIGGLER_INTERVAL_MS) {
        jiggler_timer = timer_read32();
        send_mouse_delta(JIGGLER_STEP, 0);
        send_mouse_delta(-JIGGLER_STEP, 0);
        send_mouse_delta(0, 0);
    }
}


typedef struct {
    bool is_press_action;
    uint8_t step;
} tap;

enum {
    SINGLE_TAP = 1,      
    SINGLE_HOLD,         
    DOUBLE_TAP,          
    DOUBLE_HOLD,         
    DOUBLE_SINGLE_TAP,   
    MORE_TAPS            
};

static tap dance_state[1];

uint8_t dance_step(tap_dance_state_t *state);

uint8_t dance_step(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return SINGLE_TAP;
        else return SINGLE_HOLD;
    } else if (state->count == 2) {
        if (state->interrupted) return DOUBLE_SINGLE_TAP;
        else if (state->pressed) return DOUBLE_HOLD;
        else return DOUBLE_TAP;
    }
    return MORE_TAPS;
}


void on_dance_0(tap_dance_state_t *state, void *user_data);
void dance_0_finished(tap_dance_state_t *state, void *user_data);
void dance_0_reset(tap_dance_state_t *state, void *user_data);

void on_dance_0(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_BSPC);
        tap_code16(KC_BSPC);
        tap_code16(KC_BSPC);
    }
    if(state->count > 3) {
        tap_code16(KC_BSPC);
    }
}

void dance_0_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[0].step = dance_step(state);
    switch (dance_state[0].step) {
        case SINGLE_TAP: register_code16(KC_BSPC); break;
        case SINGLE_HOLD: layer_on(4); break;
        case DOUBLE_TAP: register_code16(KC_BSPC); register_code16(KC_BSPC); break;
        case DOUBLE_HOLD: register_code16(KC_BSPC); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_BSPC); register_code16(KC_BSPC);
    }
}

void dance_0_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[0].step) {
        case SINGLE_TAP: unregister_code16(KC_BSPC); break;
        case SINGLE_HOLD:
          layer_off(4);
        break;
        case DOUBLE_TAP: unregister_code16(KC_BSPC); break;
        case DOUBLE_HOLD: unregister_code16(KC_BSPC); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_BSPC); break;
    }
    dance_state[0].step = 0;
}

tap_dance_action_t tap_dance_actions[] = {
        [DANCE_0] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_0, dance_0_finished, dance_0_reset),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (keycode == OSMODE_TOGG) {
      if (record->event.pressed) {
          os_override = (os_override_t)((os_override + 1) % 3);
          eeconfig_update_user((uint32_t)os_override);
      }
      return false;
  }

  if (is_linux_mode()) {
      switch (keycode) {
          case KC_MAC_COPY:  if (record->event.pressed) tap_code16(LCTL(KC_C)); return false;
          case KC_MAC_CUT:   if (record->event.pressed) tap_code16(LCTL(KC_X)); return false;
          case KC_MAC_PASTE: if (record->event.pressed) tap_code16(LCTL(KC_V)); return false;
          case KC_MAC_UNDO:  if (record->event.pressed) tap_code16(LCTL(KC_Z)); return false;
      }
  }

  switch (keycode) {
  case QK_MODS ... QK_MODS_MAX:
    // Mouse and consumer keys (volume, media) with modifiers work inconsistently across operating systems,
    // this makes sure that modifiers are always applied to the key that was pressed.
    if (IS_MOUSE_KEYCODE(QK_MODS_GET_BASIC_KEYCODE(keycode)) || IS_CONSUMER_KEYCODE(QK_MODS_GET_BASIC_KEYCODE(keycode))) {
      if (record->event.pressed) {
        add_mods(QK_MODS_GET_MODS(keycode));
        send_keyboard_report();
        wait_ms(2);
        register_code(QK_MODS_GET_BASIC_KEYCODE(keycode));
        return false;
      } else {
        wait_ms(2);
        del_mods(QK_MODS_GET_MODS(keycode));
      }
    }
    break;
    case MAC_MISSION_CONTROL:
      HCS(0x29F);

    case KC_MS_JIGGLER_TOGGLE:
        if (record->event.pressed) {
            jiggler_active = !jiggler_active;
            if (jiggler_active) {
                jiggler_timer = timer_read32();
                jiggler_confirm_active = true;
                jiggler_confirm_step = 0;
                jiggler_confirm_timer = timer_read32();
            } else {
                jiggler_confirm_active = false;
            }
        }
        return false;

    case RGB_SLD:
        if (record->event.pressed) {
            rgblight_mode(1);
        }
        return false;
  }
  return true;
}
