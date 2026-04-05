#include QMK_KEYBOARD_H
#ifdef AUDIO_ENABLE
#include "muse.h"
#endif
#include "eeprom.h"
#include "i18n.h"
#include "os_detection.h"
#ifndef ZSA_SAFE_RANGE
#define ZSA_SAFE_RANGE SAFE_RANGE
#endif

enum custom_keycodes {
  RGB_SLD = ZSA_SAFE_RANGE,
  MAC_MISSION_CONTROL,
  MAC_SPOTLIGHT,
  MAC_DND,
  MAC_LOCK,
  OSMODE_TOGG,
  KC_MS_JIGGLER_TOGGLE,
};


enum tap_dance_codes {
  DANCE_0,
};

enum planck_layers {
  _BASE,
  _LOWER,
  _RAISE,
  _ADJUST,
  _LAYER4,
  _LAYER5,
  _LAYER6,
  _LAYER7,
  _LAYER8,
  _LAYER9,
};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)

typedef enum { OS_AUTO = 0, OS_FORCE_MAC = 1, OS_FORCE_LINUX = 2 } os_override_t;

static os_variant_t  detected_os = OS_UNSURE;
static os_override_t os_override  = OS_AUTO;

static bool     jiggler_active = false;
static uint32_t jiggler_timer  = 0;
static bool     jiggler_confirm_active = false;
static uint8_t  jiggler_confirm_step   = 0;
static uint32_t jiggler_confirm_timer  = 0;

enum {
    JIGGLER_INTERVAL_MS = 30000,
    JIGGLER_STEP        = 10,
    JIGGLER_CONFIRM_STEP = 20,
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

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = LAYOUT_planck_grid(
    KC_Q,           KC_W,           KC_F,           KC_P,           KC_B,           KC_TRANSPARENT, KC_TRANSPARENT, KC_J,           KC_L,           KC_U,           KC_Y,           KC_QUOTE,       
    MT(MOD_LGUI, KC_A),MT(MOD_LALT, KC_R),MT(MOD_LCTL, KC_S),MT(MOD_LSFT, KC_T),KC_G,           KC_TRANSPARENT, KC_TRANSPARENT, KC_M,           MT(MOD_LSFT, KC_N),MT(MOD_LCTL, KC_E),MT(MOD_LALT, KC_I),MT(MOD_LGUI, KC_O),
    KC_Z,           MT(MOD_RALT, KC_X),MT(MOD_RCTL, KC_C),KC_D,           KC_V,           KC_TRANSPARENT, KC_TRANSPARENT, KC_K,           KC_H,           MT(MOD_RCTL, KC_COMMA),MT(MOD_RALT, KC_DOT),KC_SLASH,       
    KC_TRANSPARENT, KC_TRANSPARENT, LT(6, KC_ESCAPE),LT(4, KC_SPACE),LT(5, KC_TAB),  KC_TRANSPARENT, KC_NO,          LT(8, KC_ENTER),TD(DANCE_0),    LT(9, KC_DELETE),KC_TRANSPARENT, KC_TRANSPARENT
  ),

  [_LOWER] = LAYOUT_planck_grid(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),

  [_RAISE] = LAYOUT_planck_grid(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),

  [_ADJUST] = LAYOUT_planck_grid(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),

  [_LAYER4] = LAYOUT_planck_grid(
    MAC_SPOTLIGHT,  KC_MAC_UNDO,    LGUI(KC_Y),     CW_TOGG,        QK_BOOT,        KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_MAC_PASTE,   KC_MAC_COPY,    KC_MAC_CUT,     KC_NO,          
    KC_LEFT_GUI,    KC_LEFT_ALT,    KC_LEFT_CTRL,   KC_LEFT_SHIFT,  KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT, KC_CAPS,        KC_LEFT,        KC_DOWN,        KC_UP,          KC_RIGHT,       
    KC_NO,          KC_RIGHT_ALT,   KC_NO,          KC_NO,          KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT, KC_ESCAPE,      KC_HOME,        KC_PGDN,        KC_PAGE_UP,     KC_END,         
    KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_TRANSPARENT, KC_NO,          KC_TRANSPARENT, KC_NO,          KC_ENTER,       KC_BSPC,        KC_DELETE,      KC_TRANSPARENT, KC_TRANSPARENT
  ),

  [_LAYER5] = LAYOUT_planck_grid(
    QK_CLEAR_EEPROM,KC_MAC_CUT,     KC_MAC_COPY,    KC_MAC_PASTE,   QK_BOOT,        KC_TRANSPARENT, KC_TRANSPARENT, KC_MAC_UNDO,    KC_ESCAPE,      MS_UP,       TOGGLE_LAYER_COLOR,RM_TOGG,        
    KC_LEFT_GUI,    KC_LEFT_ALT,    KC_LEFT_CTRL,   KC_LEFT_SHIFT,  KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          MS_LEFT,     MS_DOWN,     MS_RGHT,    RM_NEXT,
    MAC_MISSION_CONTROL,KC_RIGHT_ALT,   KC_NO,          KC_NO,          KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT, KC_MS_JIGGLER_TOGGLE, MS_WHLL,  MS_WHLD,  MS_WHLU,    MS_WHLR, 
    KC_MS_JIGGLER_TOGGLE, KC_TRANSPARENT, KC_NO,          KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          MS_BTN2,     MS_BTN1,     MS_BTN3,     KC_TRANSPARENT, KC_TRANSPARENT
  ),

  [_LAYER6] = LAYOUT_planck_grid(
    KC_NO,          RM_VALD,        RM_VALU,        KC_NO,          QK_BOOT,        KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          MU_TOGG,        MU_NEXT,        AU_TOGG,        KC_NO,          
    KC_LEFT_GUI,    KC_LEFT_ALT,    KC_LEFT_CTRL,   KC_LEFT_SHIFT,  OSMODE_TOGG,    KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_MEDIA_PREV_TRACK,KC_AUDIO_VOL_DOWN,KC_AUDIO_VOL_UP,KC_MEDIA_NEXT_TRACK,
    KC_NO,          KC_RIGHT_ALT,   KC_NO,          KC_NO,          KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_NO,          KC_TRANSPARENT, KC_NO,          KC_MEDIA_STOP,  KC_MEDIA_PLAY_PAUSE,KC_AUDIO_MUTE,  KC_TRANSPARENT, KC_TRANSPARENT
  ),

  [_LAYER7] = LAYOUT_planck_grid(
    KC_LBRC,        KC_7,           KC_8,           KC_9,           KC_RBRC,        KC_TRANSPARENT, KC_TRANSPARENT, QK_BOOT,        KC_KP_PLUS,     KC_KP_MINUS,    KC_KP_ASTERISK, KC_SLASH,       
    KC_SCLN,        KC_4,           KC_5,           KC_6,           KC_EQUAL,       KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_LEFT_SHIFT,  KC_LEFT_CTRL,   KC_LEFT_ALT,    KC_LEFT_GUI,    
    KC_GRAVE,       KC_1,           KC_2,           KC_3,           KC_BSLS,        KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_NO,          KC_NO,          KC_RIGHT_ALT,   QK_LLCK,        
    KC_TRANSPARENT, KC_TRANSPARENT, KC_DOT,         KC_0,           KC_NO,          KC_TRANSPARENT, KC_NO,          KC_NO,          KC_TRANSPARENT, KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT
  ),

  [_LAYER8] = LAYOUT_planck_grid(
    KC_LCBR,        KC_AMPR,        KC_NO,          KC_LPRN,        KC_RCBR,        KC_TRANSPARENT, KC_TRANSPARENT, QK_BOOT,        KC_NO,          KC_NO,          KC_NO,          KC_NO,          
    KC_COLN,        KC_DLR,         KC_PERC,        KC_CIRC,        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_LEFT_SHIFT,  KC_LEFT_CTRL,   KC_LEFT_ALT,    KC_LEFT_GUI,    
    KC_TILD,        KC_EXLM,        KC_AT,          KC_HASH,        KC_PIPE,        KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_NO,          KC_NO,          KC_RIGHT_ALT,   KC_NO,          
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_RPRN,        KC_UNDS,        KC_TRANSPARENT, KC_NO,          KC_TRANSPARENT, KC_NO,          KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT
  ),

  [_LAYER9] = LAYOUT_planck_grid(
    KC_F12,         KC_F7,          KC_F8,          KC_F9,          KC_PSCR,        KC_TRANSPARENT, KC_TRANSPARENT, QK_BOOT,        QK_DYNAMIC_TAPPING_TERM_PRINT,QK_DYNAMIC_TAPPING_TERM_DOWN,QK_DYNAMIC_TAPPING_TERM_UP,MAC_LOCK,       
    KC_F11,         KC_F4,          KC_F5,          KC_F6,          KC_SCRL,        KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_LEFT_SHIFT,  KC_LEFT_CTRL,   KC_LEFT_ALT,    KC_LEFT_GUI,    
    KC_F10,         KC_F1,          KC_F2,          KC_F3,          KC_PAUSE,       KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,          KC_NO,          KC_NO,          KC_RIGHT_ALT,   MAC_DND,        
    KC_TRANSPARENT, KC_TRANSPARENT, KC_APPLICATION, KC_SPACE,       KC_TAB,         KC_TRANSPARENT, KC_NO,          KC_NO,          KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
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
    [0] = { {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {74,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {152,255,255}, {74,255,255}, {74,255,255}, {74,255,255}, {152,255,255}, {152,255,255}, {201,116,201}, {4,207,206}, {121,155,206}, {152,255,255}, {41,255,255}, {219,255,255}, {0,219,201}, {152,255,255}, {152,255,255} },

    [4] = { {41,255,253}, {89,202,181}, {89,202,181}, {234,225,224}, {4,213,228}, {26,210,189}, {26,210,189}, {0,0,0}, {134,113,189}, {134,113,189}, {134,113,189}, {0,0,0}, {134,218,204}, {134,218,204}, {134,218,204}, {134,218,204}, {0,0,0}, {26,210,189}, {26,210,189}, {0,0,0}, {26,210,189}, {26,210,189}, {26,210,189}, {26,210,189}, {0,0,0}, {134,218,204}, {0,0,0}, {0,0,0}, {0,0,0}, {26,210,189}, {26,210,189}, {234,225,224}, {72,218,204}, {72,218,204}, {72,218,204}, {72,218,204}, {26,210,189}, {26,210,189}, {0,0,0}, {0,0,0}, {0,0,0}, {26,210,189}, {41,255,255}, {203,244,245}, {0,245,245}, {26,210,189}, {26,210,189} },

    [5] = { {0,0,0}, {197,236,195}, {197,236,195}, {197,236,195}, {4,213,228}, {203,244,245}, {203,244,245}, {89,202,181}, {89,202,181}, {152,255,255}, {30,174,189}, {92,218,204}, {134,155,207}, {134,155,207}, {134,155,207}, {134,155,207}, {0,0,0}, {203,244,245}, {203,244,245}, {0,0,0}, {152,255,255}, {152,255,255}, {152,255,255}, {92,218,204}, {0,0,0}, {134,155,207}, {0,0,0}, {0,0,0}, {0,0,0}, {203,244,245}, {203,244,245}, {0,245,245}, {203,244,245}, {203,244,245}, {203,244,245}, {203,244,245}, {203,244,245}, {203,244,245}, {0,0,0}, {0,0,0}, {0,0,0}, {203,244,245}, {41,255,255}, {41,255,255}, {41,255,255}, {203,244,245}, {203,244,245} },

    [6] = { {0,0,0}, {41,255,253}, {41,255,253}, {0,0,0}, {4,213,228}, {70,221,91}, {70,221,91}, {0,0,0}, {54,198,148}, {54,198,148}, {54,198,148}, {0,0,0}, {134,155,207}, {134,155,207}, {134,155,207}, {134,155,207}, {0,0,0}, {70,221,91}, {70,221,91}, {0,0,0}, {140,55,178}, {140,55,178}, {140,55,178}, {140,55,178}, {0,0,0}, {134,155,207}, {0,0,0}, {0,0,0}, {0,0,0}, {70,221,91}, {70,221,91}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {70,221,91}, {70,221,91}, {0,0,0}, {0,0,0}, {0,0,0}, {140,55,178}, {140,55,178}, {140,55,178}, {140,55,178}, {70,221,91}, {70,221,91} },

    [7] = { {0,0,0}, {242,218,201}, {242,218,201}, {242,218,201}, {0,0,0}, {242,218,201}, {242,218,201}, {4,213,228}, {41,255,255}, {41,255,255}, {41,255,255}, {41,255,255}, {28,114,212}, {242,218,201}, {242,218,201}, {242,218,201}, {134,139,195}, {242,218,201}, {242,218,201}, {0,0,0}, {134,155,207}, {134,155,207}, {134,155,207}, {134,155,207}, {28,114,212}, {242,218,201}, {242,218,201}, {242,218,201}, {0,0,0}, {242,218,201}, {242,218,201}, {0,0,0}, {0,0,0}, {0,0,0}, {134,155,207}, {17,218,204}, {242,218,201}, {242,218,201}, {28,114,212}, {242,218,201}, {0,0,0}, {242,218,201}, {0,0,0}, {0,0,0}, {0,0,0}, {242,218,201}, {242,218,201} },

    [8] = { {220,83,195}, {0,0,0}, {0,0,0}, {121,155,206}, {220,83,195}, {220,83,195}, {220,83,195}, {4,213,228}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {220,83,195}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {220,83,195}, {220,83,195}, {0,0,0}, {134,155,207}, {134,155,207}, {134,155,207}, {134,155,207}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {220,83,195}, {220,83,195}, {0,0,0}, {0,0,0}, {0,0,0}, {134,155,207}, {0,0,0}, {220,83,195}, {220,83,195}, {0,0,0}, {121,155,206}, {0,0,0}, {220,83,195}, {0,0,0}, {0,0,0}, {0,0,0}, {220,83,195}, {220,83,195} },

    [9] = { {45,169,235}, {45,169,235}, {45,169,235}, {45,169,235}, {89,202,181}, {41,255,255}, {41,255,255}, {4,213,228}, {220,83,195}, {220,83,195}, {220,83,195}, {92,218,204}, {45,169,235}, {45,169,235}, {45,169,235}, {45,169,235}, {0,0,0}, {41,255,255}, {41,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {45,169,235}, {45,169,235}, {45,169,235}, {45,169,235}, {0,0,0}, {41,255,255}, {41,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {24,198,221}, {41,255,255}, {41,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {41,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {41,255,255}, {41,255,255} },

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
      case 4:
        set_layer_color(4);
        break;
      case 5:
        set_layer_color(5);
        break;
      case 6:
        set_layer_color(6);
        break;
      case 7:
        set_layer_color(7);
        break;
      case 8:
        set_layer_color(8);
        break;
      case 9:
        set_layer_color(9);
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

  if (capslock_active && biton32(layer_state) == 4) {
    RGB rgb = hsv_to_rgb_with_value((HSV) { 245, 233, 224 });
    rgb_matrix_set_color( 19, rgb.r, rgb.g, rgb.b );
  } 
  if (scrolllock_active && biton32(layer_state) == 9) {
    RGB rgb = hsv_to_rgb_with_value((HSV) { 92, 218, 204 });
    rgb_matrix_set_color( 16, rgb.r, rgb.g, rgb.b );
  }
  if (jiggler_active) {
    RGB rgb = hsv_to_rgb_with_value((HSV){ 85, 255, 220 });
    rgb_matrix_set_color(36, rgb.r, rgb.g, rgb.b);
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
        case SINGLE_HOLD: layer_on(7); break;
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
          if(!is_layer_locked(7)) {
            layer_off(7);
          }
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
          case LGUI(KC_Y):   if (record->event.pressed) tap_code16(LCTL(KC_Y)); return false;
      }
  }

  switch (keycode) {
    case MAC_MISSION_CONTROL:
      HCS(0x29F);
    case MAC_SPOTLIGHT:
      HCS(0x221);
    case MAC_DND:
      HSS(0x9B);
    case MAC_LOCK:
      HCS(0x19E);

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

#ifdef AUDIO_ENABLE
bool muse_mode = false;
uint8_t last_muse_note = 0;
uint16_t muse_counter = 0;
uint8_t muse_offset = 70;
uint16_t muse_tempo = 50;

void encoder_update(bool clockwise) {
    if (muse_mode) {
        if (IS_LAYER_ON(_RAISE)) {
            if (clockwise) {
                muse_offset++;
            } else {
                muse_offset--;
            }
        } else {
            if (clockwise) {
                muse_tempo+=1;
            } else {
                muse_tempo-=1;
            }
        }
    } else {
        if (clockwise) {
        #ifdef MOUSEKEY_ENABLE
            register_code(MS_WHLD);
            unregister_code(MS_WHLD);
        #else
            register_code(KC_PGDN);
            unregister_code(KC_PGDN);
        #endif
        } else {
        #ifdef MOUSEKEY_ENABLE
            register_code(MS_WHLU);
            unregister_code(MS_WHLU);
        #else
            register_code(KC_PGUP);
            unregister_code(KC_PGUP);
        #endif
        }
    }
}

void matrix_scan_user(void) {
#ifdef AUDIO_ENABLE
    if (muse_mode) {
        if (muse_counter == 0) {
            uint8_t muse_note = muse_offset + SCALE[muse_clock_pulse()];
            if (muse_note != last_muse_note) {
                stop_note(compute_freq_for_midi_note(last_muse_note));
                play_note(compute_freq_for_midi_note(muse_note), 0xF);
                last_muse_note = muse_note;
            }
        }
        muse_counter = (muse_counter + 1) % muse_tempo;
    }
#endif
}

bool music_mask_user(uint16_t keycode) {
    switch (keycode) {
    case RAISE:
    case LOWER:
        return false;
    default:
        return true;
    }
}
#endif

uint16_t layer_state_set_user(uint16_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}
