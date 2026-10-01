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
#define HERDR_CC_SLOT_FIRST 112
#define HERDR_CC_ECHO       116
#define HERDR_PROTOCOL      3

#ifndef HERDR_TIMEOUT_MS
#    define HERDR_TIMEOUT_MS 3000
#endif
#define HERDR_VELOCITY_TAP  127

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

static uint8_t  herdr_slots[HERDR_SLOT_COUNT] = {HERDR_EMPTY, HERDR_EMPTY, HERDR_EMPTY, HERDR_EMPTY};
static uint8_t  herdr_colors[HERDR_SLOT_COUNT];
static uint32_t herdr_last_seen;
static bool     herdr_seen;

bool herdr_is_connected(void) {
    return herdr_seen && timer_elapsed32(herdr_last_seen) <= HERDR_TIMEOUT_MS;
}

uint8_t herdr_slot_state(uint8_t slot) { return herdr_slots[slot]; }

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
        herdr_seen      = true;
        herdr_last_seen = timer_read32();
        midi_send_cc(device, HERDR_MIDI_CHANNEL, HERDR_CC_ECHO, HERDR_PROTOCOL);
        return;
    }
    if (!herdr_is_connected()) {
        return;
    }

    if (number >= HERDR_CC_SLOT_FIRST && number < HERDR_CC_SLOT_FIRST + HERDR_SLOT_COUNT) {
        herdr_slots[number - HERDR_CC_SLOT_FIRST]   = value & 0x07;
        herdr_colors[number - HERDR_CC_SLOT_FIRST]  = (value >> 5) & 0x03;
    }
}

void herdr_init(void) {
    midi_register_cc_callback(&midi_device, herdr_midi_cc);
}

static uint8_t herdr_note(uint16_t keycode) {
    if (keycode < QK_USER_0 || keycode - QK_USER_0 >= ARRAY_SIZE(herdr_notes)) {
        return 0;
    }
    return herdr_notes[keycode - QK_USER_0];
}

bool process_record_herdr(uint16_t keycode, keyrecord_t *record) {
    uint8_t note = herdr_note(keycode);
    if (!note) {
        return true;
    }
    if (!record->event.pressed) {
        midi_send_noteoff(&midi_device, HERDR_MIDI_CHANNEL, note, 0);
        return false;
    }

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

// Blocked agents blink every 250 ms.
static bool herdr_blocked_blink_on(void) {
    return (timer_read32() / 250) & 1;
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

static void herdr_set_state_led(uint8_t led, uint8_t state, uint8_t color) {
    uint8_t hue = herdr_agent_hues[color];
    switch (state) {
        case HERDR_IDLE:
            set_led_hsv(led, (HSV){hue, MIRYOKU_CTP_SAT, 48});
            break;
        case HERDR_WORKING:
            set_led_hsv(led, (HSV){hue, MIRYOKU_CTP_SAT, herdr_breath_value()});
            break;
        case HERDR_BLOCKED:
            if (herdr_blocked_blink_on()) {
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

// One host connection LED (dim yellow connected, dim red disconnected) and
// HERDR_SLOT_COUNT agent slot LEDs that stay dark while disconnected.  Once the
// LEDs idle, only slots that need attention stay lit.
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
        set_led_hsv(connection_led, (HSV){CTP_YELLOW, MIRYOKU_CTP_SAT, 64});
    }
    for (uint8_t i = 0; i < HERDR_SLOT_COUNT; ++i) {
        if (!idle || herdr_slots[i] != HERDR_IDLE) {
            herdr_set_state_led(slot_leds[i], herdr_slots[i], herdr_colors[i]);
        }
    }
}
#endif

#endif
