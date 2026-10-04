#!/usr/bin/env python3
"""Compile and exercise the real jiggler with a fake clock and mouse device.

CC=cc python3 users/manna-harbour_miryoku/tests/test_mouse_jiggler.py
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
#define SAFE_RANGE QK_USER_0
// Only the fields used by this module are needed for the host test.
typedef struct { struct { bool pressed; } event; } keyrecord_t;
typedef struct { uint8_t buttons; int8_t x, y, v, h; } report_mouse_t;
static uint16_t now;
static unsigned sent;
static report_mouse_t held, last_report;
static uint16_t timer_read(void) { return now; }
static uint16_t timer_elapsed(uint16_t since) { return now - since; }
static report_mouse_t mousekey_get_report(void) { return held; }
static void host_mouse_send(report_mouse_t *report) {
    last_report = *report;
    ++sent;
}
"""
CHECK = r"""
static void toggle(bool pressed) {
    keyrecord_t record = {.event.pressed = pressed};
    process_record_mouse_jiggler(MJ_TOGG, &record);
}
int main(void) {
    assert(!mouse_jiggler_is_enabled());
    now = 6000;
    housekeeping_task_mouse_jiggler();
    assert(sent == 0);
    toggle(true);
    assert(mouse_jiggler_is_enabled());
    toggle(false);
    assert(mouse_jiggler_is_enabled());
    now += 5000;
    housekeeping_task_mouse_jiggler();
    assert(sent == 0);

    // Each button and combinations must survive a jiggle during a drag.
    for (unsigned buttons = 0; buttons < 8; ++buttons) {
        held = (report_mouse_t){.buttons = buttons, .x = 9, .y = -4, .v = 1, .h = -1};
        now += 5001;
        housekeeping_task_mouse_jiggler();
        assert(sent == buttons + 1);
        assert(last_report.buttons == buttons);
        assert(last_report.x == (buttons % 2 ? -3 : 3));
        assert(last_report.y == 0 && last_report.v == 0 && last_report.h == 0);
        assert(held.buttons == buttons && held.x == 9 && held.y == -4);
    }
    toggle(true);
    toggle(false);
    assert(!mouse_jiggler_is_enabled());
    now += 5001;
    housekeeping_task_mouse_jiggler();
    assert(sent == 8);

    // Re-enabling starts a fresh interval, including across timer rollover.
    now = 65000;
    toggle(true);
    now += 5001;
    housekeeping_task_mouse_jiggler();
    assert(sent == 9);
    assert(last_report.buttons == held.buttons);
    puts("Mouse jiggler checks passed");
}
"""

with tempfile.TemporaryDirectory() as directory:
    temp = Path(directory)
    # Use QMK's actual user keycode values, not invented test values.
    keycodes = (USER.parents[1] / "quantum/keycodes.h").read_text()
    definitions = "\n".join(f"#define {name} {value}" for name, value in
                            re.findall(r"\b(QK_USER_\d+) = (0x[0-9A-Fa-f]+)", keycodes))
    (temp / "quantum.h").write_text(STUB + definitions + "\n")
    (temp / "mousekey.h").write_text('#include "quantum.h"\n')
    config = (USER / "custom_config.h").read_text()
    controls = re.findall(r"#define\s+(RGB_LAYER_COLORS|AG_\w+)\s+QK_USER_\d+", config)
    collisions = "\n".join(f'_Static_assert(MJ_TOGG != {name}, "MJ_TOGG collides with {name}");'
                           for name in controls)
    (temp / "check.c").write_text('#define RGB_MATRIX_ENABLE\n#define HERDR_AGENT_ENABLE\n'
                                 f'#include "{USER / "custom_config.h"}"\n'
                                 f'#include "{USER / "mouse_jiggler.c"}"\n' + collisions + "\n" + CHECK)
    compiler = shlex.split(os.environ.get("CC", "cc"))
    subprocess.run(compiler + ["-std=c11", "-Wall", "-Wextra", "-Werror", "-I", str(temp),
                              str(temp / "check.c"), "-o", str(temp / "check")], check=True)
    subprocess.run([str(temp / "check")], check=True)
