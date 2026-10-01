// Herdr agent controls and status shared by the Miryoku ZSA keymaps.
//
// Controls go to the qmk-herdr bridge as Note On/Off on MIDI channel 15.  Notes
// have no reserved numbers, unlike CC 100/101 (RPN select) and CC 120-127
// (channel mode messages), which MIDI routers may filter or act on.  Status
// comes back from the bridge as CCs on the same channel.

#ifdef HERDR_AGENT_ENABLE

#include "herdr.h"
#include "manna-harbour_miryoku.h"
#include "qmk_midi.h"
#ifdef RGB_MATRIX_ENABLE
#    include "miryoku_rgb.h"
#endif

#define HERDR_MIDI_CHANNEL  14 // QMK channels are zero-based: MIDI channel 15
#define HERDR_CC_HEARTBEAT  110
#define HERDR_CC_STATE      111
#define HERDR_CC_SLOT_FIRST 112
#define HERDR_CC_ECHO       116
#define HERDR_CC_RISK       117
#define HERDR_CC_ACTIVE     119 // bridge to keyboard: 1 active, 0 standby
#define HERDR_PROTOCOL      3

#ifndef HERDR_TIMEOUT_MS
#    define HERDR_TIMEOUT_MS 3000
#endif
// Reject and Clear only fire when tapped twice within this window.
#ifndef HERDR_CONFIRM_TERM
#    define HERDR_CONFIRM_TERM TAPPING_TERM
#endif
// Swallow a repeated chime, e.g. a state message redelivered by RTP-MIDI.
#define HERDR_CHIME_DEBOUNCE_MS 1000
#define HERDR_DROPPED_FLASH_MS  300
#define HERDR_VELOCITY_TAP  127

// eeconfig user bits 0-1 hold the OS override, and bits 2 and 4 held the
// retired spinner and sort toggles; a zero bit keeps sounds on.
#define HERDR_EE_SOUNDS_OFF    (1 << 3)
#define HERDR_EE_MASK          HERDR_EE_SOUNDS_OFF

// The state CC's aggregate state and its any-working and overflow flags (bits
// 0-4) are not shown; the board only uses the chime flags.
enum herdr_flags {
    HERDR_CHIME_DONE  = 1 << 5,
    HERDR_CHIME_BLOCK = 1 << 6,
};

// Keycode -> note.  Numbers match the bridge's control table; 0 means no note.
static const uint8_t herdr_notes[] = {
    [AG_WS_PREV - QK_USER_0]      = 100,
    [AG_WS_NEXT - QK_USER_0]      = 101,
    [AG_TAB_PREV - QK_USER_0]     = 102,
    [AG_TAB_NEXT - QK_USER_0]     = 103,
    [AG_PANE_LEFT - QK_USER_0]    = 104,
    [AG_PANE_DOWN - QK_USER_0]    = 105,
    [AG_PANE_UP - QK_USER_0]      = 106,
    [AG_PANE_RIGHT - QK_USER_0]   = 107,
    [AG_AGENT_PICKER - QK_USER_0] = 108,
    [AG_SCRATCHPAD - QK_USER_0]   = 109,
    [AG_WS_NEW - QK_USER_0]       = 116,
    [AG_TAB_NEW - QK_USER_0]      = 117,
    [AG_LAZYGIT - QK_USER_0]      = 118,
    [AG_PALETTE - QK_USER_0]      = 119,
    [AG_PANE_ZOOM - QK_USER_0]    = 120,
    [AG_HUNK - QK_USER_0]         = 121,
    [AG_AGENT_NEXT - QK_USER_0]   = 122,
    [AG_ACCEPT - QK_USER_0]       = 124,
    [AG_REJECT - QK_USER_0]       = 125,
    [AG_PROMPT - QK_USER_0]       = 126,
    [AG_CLEAR - QK_USER_0]        = 127,
    [AG_AGENT_PREV - QK_USER_0]   = 110,
    [AG_AGENT_URGENT - QK_USER_0] = 111,
};

static uint32_t herdr_settings;
static uint8_t  herdr_slots[HERDR_SLOT_COUNT] = {HERDR_EMPTY, HERDR_EMPTY, HERDR_EMPTY, HERDR_EMPTY};
static uint8_t  herdr_reasons[HERDR_SLOT_COUNT];
static uint8_t  herdr_colors[HERDR_SLOT_COUNT];
static uint8_t  herdr_risk;
static uint32_t herdr_last_seen;
static bool     herdr_seen;
static bool     herdr_standby;
static uint32_t herdr_last_chime;
static bool     herdr_chimed;
static uint16_t herdr_armed_key = KC_NO;
static uint32_t herdr_armed_time;
#ifdef RGB_MATRIX_ENABLE
static uint8_t  herdr_dropped_led = NO_LED;
static uint32_t herdr_dropped_time;
#endif

#ifdef AUDIO_ENABLE
static float herdr_connected_song[][2] = {{NOTE_E6, 8}, {NOTE_A6, 8}};
static float herdr_done_song[][2]      = {{NOTE_E6, 4}, {NOTE_A6, 4}, {NOTE_E7, 8}};
static float herdr_blocked_song[][2]   = {{NOTE_A5, 8}, {NOTE_A5, 8}};
#    define HERDR_PLAY(song) PLAY_SONG(song)
#else
#    define HERDR_PLAY(song)
#endif

bool herdr_sounds_are_enabled(void) { return !(herdr_settings & HERDR_EE_SOUNDS_OFF); }

bool herdr_is_connected(void) {
    return herdr_seen && timer_elapsed32(herdr_last_seen) <= HERDR_TIMEOUT_MS;
}

// Another keyboard drives the bridge; this one mirrors state until claimed.
static bool herdr_is_standby(void) { return herdr_standby && herdr_is_connected(); }

uint8_t herdr_slot_state(uint8_t slot) { return herdr_slots[slot]; }
uint8_t herdr_accept_risk(void) { return herdr_is_connected() ? herdr_risk : HERDR_RISK_NONE; }

bool herdr_is_armed(uint16_t keycode) {
    return herdr_armed_key == keycode && timer_elapsed32(herdr_armed_time) < HERDR_CONFIRM_TERM;
}

static void herdr_chime(uint8_t value) {
    if (!(value & (HERDR_CHIME_BLOCK | HERDR_CHIME_DONE)) || !herdr_sounds_are_enabled()) {
        return;
    }
    if (herdr_chimed && timer_elapsed32(herdr_last_chime) < HERDR_CHIME_DEBOUNCE_MS) {
        return;
    }
    herdr_chimed     = true;
    herdr_last_chime = timer_read32();
    if (value & HERDR_CHIME_BLOCK) {
        HERDR_PLAY(herdr_blocked_song);
    } else {
        HERDR_PLAY(herdr_done_song);
    }
}

static void herdr_midi_cc(MidiDevice *device, uint8_t channel, uint8_t number, uint8_t value) {
    if (channel != HERDR_MIDI_CHANNEL) {
        return;
    }

    // Only a heartbeat with the matching protocol counts as a connection; the
    // bridge sends one ahead of every status frame.
    if (number == HERDR_CC_HEARTBEAT) {
        if (value != HERDR_PROTOCOL) {
            return;
        }
        if (!herdr_is_connected()) {
            // A bridge that never sends CC 119 must not inherit an old standby.
            herdr_standby = false;
            if (herdr_sounds_are_enabled()) {
                HERDR_PLAY(herdr_connected_song);
            }
        }
        herdr_seen      = true;
        herdr_last_seen = timer_read32();
        midi_send_cc(device, HERDR_MIDI_CHANNEL, HERDR_CC_ECHO, HERDR_PROTOCOL);
        return;
    }
    if (!herdr_is_connected()) {
        return;
    }

    if (number == HERDR_CC_STATE) {
        herdr_chime(value);
    } else if (number >= HERDR_CC_SLOT_FIRST && number < HERDR_CC_SLOT_FIRST + HERDR_SLOT_COUNT) {
        herdr_slots[number - HERDR_CC_SLOT_FIRST]   = value & 0x07;
        herdr_reasons[number - HERDR_CC_SLOT_FIRST] = (value >> 3) & 0x03;
        herdr_colors[number - HERDR_CC_SLOT_FIRST]  = (value >> 5) & 0x03;
    } else if (number == HERDR_CC_RISK) {
        herdr_risk = value <= HERDR_RISK_HIGH ? value : HERDR_RISK_UNKNOWN;
    } else if (number == HERDR_CC_ACTIVE) {
        herdr_standby = value == 0;
    }
}

void herdr_init(void) {
    herdr_settings = eeconfig_read_user() & HERDR_EE_MASK;
    midi_register_cc_callback(&midi_device, herdr_midi_cc);
}

static void herdr_toggle_setting(uint32_t bit) {
    herdr_settings ^= bit;
    eeconfig_update_user((eeconfig_read_user() & ~HERDR_EE_MASK) | herdr_settings);
}

static uint8_t herdr_note(uint16_t keycode) {
    if (keycode < QK_USER_0 || keycode - QK_USER_0 >= ARRAY_SIZE(herdr_notes)) {
        return 0;
    }
    return herdr_notes[keycode - QK_USER_0];
}

// Flash the key when there is no bridge to receive it.
static void herdr_flag_dropped(keyrecord_t *record) {
#ifdef RGB_MATRIX_ENABLE
    if (!herdr_is_connected() && record->event.key.row < MATRIX_ROWS && record->event.key.col < MATRIX_COLS) {
        herdr_dropped_led  = g_led_config.matrix_co[record->event.key.row][record->event.key.col];
        herdr_dropped_time = timer_read32();
    }
#endif
}

bool process_record_herdr(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed && keycode != herdr_armed_key) {
        herdr_armed_key = KC_NO;
    }

    if (keycode == AG_SOUND_TOGGLE) {
        if (record->event.pressed) {
            herdr_toggle_setting(HERDR_EE_SOUNDS_OFF);
        }
        return false;
    }

    uint8_t note = herdr_note(keycode);
    if (!note) {
        return true;
    }
    if (!record->event.pressed) {
        midi_send_noteoff(&midi_device, HERDR_MIDI_CHANNEL, note, 0);
        return false;
    }

    herdr_flag_dropped(record);

    // On a standby board the press only claims it: the bridge swallows the
    // note, so skip the arming and layer exit a real action would bring.
    if (herdr_is_standby()) {
        herdr_armed_key = KC_NO;
        midi_send_noteon(&midi_device, HERDR_MIDI_CHANNEL, note, HERDR_VELOCITY_TAP);
        return false;
    }

    // Accept needs the same confirmation when the bridge rates the approval as risky.
    bool confirm = keycode == AG_REJECT || keycode == AG_CLEAR ||
                   (keycode == AG_ACCEPT && herdr_accept_risk() == HERDR_RISK_HIGH);
    if (confirm && !herdr_is_armed(keycode)) {
        herdr_armed_key  = keycode;
        herdr_armed_time = timer_read32();
        return false;
    }
    herdr_armed_key = KC_NO;
    midi_send_noteon(&midi_device, HERDR_MIDI_CHANNEL, note, HERDR_VELOCITY_TAP);

    // These hand focus back to the agent, so return to typing.
    switch (keycode) {
        case AG_PROMPT:
        case AG_ACCEPT:
        case AG_REJECT:
        case AG_CLEAR:
            layer_off(U_AGENT);
            break;
    }
    return false;
}

#ifdef RGB_MATRIX_ENABLE
static void set_led_hsv(uint8_t led, HSV hsv) {
    RGB rgb = hsv_to_rgb_with_value(hsv);
    rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
}

// Blocked agents blink in a rhythm for their reason: steady 250 ms for a
// permission prompt or unknown reason, a double blink for a question, and a
// fast blink for an error.
static bool herdr_blocked_blink_on(uint8_t reason) {
    uint32_t now = timer_read32();
    switch (reason) {
        case HERDR_REASON_QUESTION: {
            uint16_t phase = now % 1000;
            return phase < 100 || (phase >= 200 && phase < 300);
        }
        case HERDR_REASON_ERROR:
            return (now / 100) & 1;
        default:
            return (now / 250) & 1;
    }
}

// Working agents breathe in step on a ~2 s cycle, never below about 30%
// brightness so a working slot never looks empty.
static uint8_t herdr_breath_value(void) {
    uint16_t phase = (timer_read32() / 4) % 512;
    uint8_t  level = phase < 256 ? phase : 511 - phase;
    return 80 + level * 175 / 255;
}

// Each agent on the board has its own hue, picked by the bridge; its state
// shows as dim, breathing, blinking, half, or bright in that hue.
static const uint8_t herdr_agent_hues[] = {CTP_BLUE, CTP_YELLOW, CTP_TEAL, CTP_MAUVE};

static void herdr_set_state_led(uint8_t led, uint8_t state, uint8_t reason, uint8_t color) {
    uint8_t hue = herdr_agent_hues[color];
    switch (state) {
        case HERDR_IDLE:
            set_led_hsv(led, (HSV){hue, MIRYOKU_CTP_SAT, 48});
            break;
        case HERDR_WORKING:
            set_led_hsv(led, (HSV){hue, MIRYOKU_CTP_SAT, herdr_breath_value()});
            break;
        case HERDR_BLOCKED:
            if (herdr_blocked_blink_on(reason)) {
                set_led_hsv(led, (HSV){hue, MIRYOKU_CTP_SAT, 255});
            }
            break;
        case HERDR_DONE:
            set_led_hsv(led, (HSV){hue, MIRYOKU_CTP_SAT, 255});
            break;
        case HERDR_UNKNOWN:
            set_led_hsv(led, (HSV){hue, MIRYOKU_CTP_SAT, 128});
            break;
    }
}

// One host connection LED (dim yellow active, dim blue standby, dim red
// disconnected) and HERDR_SLOT_COUNT agent slot LEDs that stay dark while
// disconnected.  Once the LEDs idle, only slots that need attention stay lit.
void herdr_render_status(uint8_t connection_led, const uint8_t *slot_leds) {
    bool idle = miryoku_leds_idle();

    rgb_matrix_set_color(connection_led, 0, 0, 0);
    for (uint8_t i = 0; i < HERDR_SLOT_COUNT; ++i) {
        rgb_matrix_set_color(slot_leds[i], 0, 0, 0);
    }

    if (!herdr_is_connected()) {
        if (!idle) {
            set_led_hsv(connection_led, (HSV){CTP_RED, MIRYOKU_CTP_SAT, 64});
        }
        return;
    }
    if (!idle) {
        // Yellow on the board the bridge follows, blue on a standby board.
        set_led_hsv(connection_led, (HSV){herdr_is_standby() ? CTP_BLUE : CTP_YELLOW, MIRYOKU_CTP_SAT, 64});
    }
    for (uint8_t i = 0; i < HERDR_SLOT_COUNT; ++i) {
        if (!idle || herdr_slots[i] != HERDR_IDLE) {
            herdr_set_state_led(slot_leds[i], herdr_slots[i], herdr_reasons[i], herdr_colors[i]);
        }
    }
}

void herdr_render_dropped_press(void) {
    if (herdr_dropped_led != NO_LED && timer_elapsed32(herdr_dropped_time) < HERDR_DROPPED_FLASH_MS) {
        set_led_hsv(herdr_dropped_led, (HSV){CTP_RED, MIRYOKU_CTP_SAT, 255});
    }
}
#endif

#endif
