#!/usr/bin/env python3
"""Run the real Herdr firmware handlers with a fake clock and MIDI device.

CC=cc python3 users/manna-harbour_miryoku/tests/test_herdr.py
"""
import os
import re
import shlex
import subprocess
import tempfile
from pathlib import Path

USER = Path(__file__).resolve().parents[1]
STUB = r"""
#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define HERDR_AGENT_ENABLE
#define KC_NO 0
#define U_AGENT 10
#define TAPPING_TERM 200
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
typedef uint32_t layer_state_t;
typedef struct { struct { bool pressed; } event; } keyrecord_t;
typedef struct { int unused; } MidiDevice;
static MidiDevice midi_device;
static uint32_t now, layer_state;
static unsigned sent, last_note, last_velocity;
layer_state_t layer_state_set_user(layer_state_t state);
static bool layer_state_is(unsigned layer) { return layer_state & (1u << layer); }
static void layer_off(unsigned layer) {
    layer_state = layer_state_set_user(layer_state & ~(1u << layer));
}
static uint32_t timer_read32(void) { return now; }
static uint32_t timer_elapsed32(uint32_t since) { return now - since; }
static uint32_t eeconfig_read_user(void) { return 0; }
static void eeconfig_update_user(uint32_t value) {}
static void midi_register_cc_callback(MidiDevice *d,
    void (*callback)(MidiDevice *, uint8_t, uint8_t, uint8_t)) {}
static void midi_send_cc(MidiDevice *d, unsigned c, unsigned n, unsigned v) {}
static void midi_send_noteoff(MidiDevice *d, unsigned c, unsigned n, unsigned v) {}
static void midi_send_noteon(MidiDevice *d, unsigned c, unsigned n, unsigned v) {
    ++sent; last_note = n; last_velocity = v;
}
"""
CHECK = r"""
static void reset(void) {
    now = 10; layer_state = 1u << U_AGENT;
    herdr_seen = true; herdr_last_seen = now; herdr_standby = false;
    herdr_held_key = herdr_armed_key = KC_NO; sent = 0;
}
static void event(uint16_t key, bool pressed) {
    keyrecord_t record = {.event.pressed = pressed};
    process_record_herdr(key, &record);
}
static void cc(unsigned number, unsigned value) {
    herdr_midi_cc(&midi_device, HERDR_MIDI_CHANNEL, number, value);
}
int main(void) {
    reset(); event(AG_PANE_LEFT, true); now += HERDR_HOLD_TERM - 1;
    assert(!herdr_is_held(AG_PANE_LEFT)); event(AG_PANE_LEFT, false);
    assert(sent == 1 && last_note == 104 && last_velocity == 127);
    reset(); event(AG_PANE_LEFT, true); now += HERDR_HOLD_TERM;
    assert(herdr_is_held(AG_PANE_LEFT)); event(AG_PANE_LEFT, false);
    assert(sent == 1 && last_velocity == 64);

    // Even off-and-on in one scan must discard the pending action.
    reset(); event(AG_PANE_LEFT, true); now += HERDR_HOLD_TERM;
    layer_off(U_AGENT);
    layer_state = layer_state_set_user(1u << U_AGENT);
    event(AG_PANE_LEFT, false); assert(sent == 0);

    // A disconnected press must not become an action after reconnection.
    reset(); herdr_seen = false; event(AG_PANE_LEFT, true);
    now += HERDR_HOLD_TERM; cc(HERDR_CC_HEARTBEAT, HERDR_PROTOCOL);
    event(AG_PANE_LEFT, false); assert(sent == 0);

    // Timeout cancels both in housekeeping and at release.
    reset(); event(AG_PANE_LEFT, true); now += HERDR_TIMEOUT_MS + 1;
    herdr_task(); assert(herdr_held_key == KC_NO);
    event(AG_PANE_LEFT, false); assert(sent == 0);
    reset(); event(AG_PANE_LEFT, true); now += HERDR_TIMEOUT_MS + 1;
    event(AG_PANE_LEFT, false); assert(sent == 0);

    // Reconnection must cancel before refreshing the heartbeat clock.
    reset(); event(AG_PANE_LEFT, true); now += HERDR_TIMEOUT_MS + 1;
    cc(HERDR_CC_HEARTBEAT, HERDR_PROTOCOL);
    event(AG_PANE_LEFT, false); assert(sent == 0);

    // A handoff away and back must not resurrect an old hold.
    reset(); event(AG_PANE_LEFT, true); now += HERDR_HOLD_TERM;
    cc(HERDR_CC_ACTIVE, 0); cc(HERDR_CC_ACTIVE, 1);
    event(AG_PANE_LEFT, false); assert(sent == 0);
    reset(); herdr_standby = true; event(AG_PANE_LEFT, true);
    assert(sent == 1 && last_velocity == 127); // Claim only.
    cc(HERDR_CC_ACTIVE, 1); now += HERDR_HOLD_TERM;
    event(AG_PANE_LEFT, false); assert(sent == 1);

    // Rolls still flush once, and the old release cannot fire again.
    reset(); event(AG_PANE_LEFT, true); now += 100;
    event(AG_PANE_RIGHT, true);
    assert(sent == 1 && last_note == 104 && last_velocity == 127);
    event(AG_PANE_LEFT, false); assert(sent == 1);
    now += HERDR_HOLD_TERM; event(AG_PANE_RIGHT, false);
    assert(sent == 2 && last_note == 107 && last_velocity == 64);
    puts("Herdr hold checks passed");
}
"""


def main():
    # Replace QMK's hardware includes only; compile the actual handlers and headers.
    firmware = re.sub(r'^#\s*include[^\n]*', '', (USER / 'herdr.c').read_text(), flags=re.M)
    with tempfile.TemporaryDirectory() as directory:
        temp = Path(directory)
        codes = '\n'.join(f'#define QK_USER_{i} {1000 + i}' for i in range(31))
        (temp / 'quantum.h').write_text(STUB + '\n' + codes)
        source = '#include "quantum.h"\n#include "custom_config.h"\n#include "herdr.h"\n' + firmware + CHECK
        for override in ([], ['-DHERDR_HOLD_TERM=500']):
            binary = temp / 'check'
            subprocess.run(shlex.split(os.environ.get('CC', 'cc')) + [
                '-std=c11', '-I', str(temp), '-I', str(USER), *override,
                '-x', 'c', '-', '-o', str(binary),
            ], input=source, text=True, check=True)
            subprocess.run([str(binary)], check=True)


if __name__ == '__main__':
    main()
