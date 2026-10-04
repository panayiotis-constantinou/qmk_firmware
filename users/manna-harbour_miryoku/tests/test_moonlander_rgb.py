#!/usr/bin/env python3
"""Check the Moonlander RGB hook against its hardware LED matrix.

CC=cc python3 users/manna-harbour_miryoku/tests/test_moonlander_rgb.py
"""
import json
import os
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BOARD = ROOT / "keyboards/zsa/moonlander"
STUB = r"""
#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define MIRYOKU_KEY_COUNT 36
#define HERDR_SLOT_COUNT 4
#define NO_LED 255
#define U_BASE 0
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
typedef struct {
    const uint8_t *keys, *free_leds, *herdr_slots;
    uint8_t free_led_count, herdr_connection, herdr_sort;
} miryoku_leds_t;
static uint16_t layer_state, default_layer_state;
static uint8_t colors[72];
static bool shared_result;
static uint8_t get_highest_layer(uint16_t state) {
    uint8_t layer = 0;
    while (state >>= 1) ++layer;
    return layer;
}
static void rgb_matrix_set_color(uint8_t led, uint8_t r, uint8_t g, uint8_t b) {
    colors[led] = r | g | b;
}
static bool miryoku_rgb_indicators(const miryoku_leds_t *map) {
    (void)map;
    // Model an effect or status overlay lighting every LED before the hook.
    for (unsigned i = 0; i < ARRAY_SIZE(colors); ++i) colors[i] = 42;
    return shared_result;
}
"""
CHECK = r"""
int main(void) {
    for (unsigned base = 0; base < 2; ++base) {
        for (unsigned result = 0; result < 2; ++result) {
            default_layer_state = 1;
            layer_state = base ? 1 : 1 << 5;
            shared_result = result;
            assert(rgb_matrix_indicators_user() == shared_result);
            for (unsigned i = 0; i < ARRAY_SIZE(colors); ++i) {
                bool dark = false;
                for (unsigned j = 0; j < ARRAY_SIZE(expected_dark); ++j) {
                    if (i == expected_dark[j]) dark = base;
                }
                assert(colors[i] == (dark ? 0 : 42));
            }
        }
    }
    // A different default layer must not be mistaken for Base.
    layer_state = 0;
    default_layer_state = 1 << 1;
    rgb_matrix_indicators_user();
    assert(colors[0] == 42);
    puts("Moonlander number-row RGB checks passed");
}
"""
layout = json.loads((BOARD / "keyboard.json").read_text())["rgb_matrix"]["layout"]
number_row = [i for i, led in enumerate(layout) if led["matrix"][0] in (0, 6)]
with tempfile.TemporaryDirectory() as directory:
    temp = Path(directory)
    (temp / "stub.h").write_text(STUB)
    for header in ("miryoku_rgb.h", "herdr.h", "manna-harbour_miryoku.h"):
        (temp / header).write_text('#include "stub.h"\n')
    expected = ", ".join(str(i) for i in number_row if i not in (25, 65))
    (temp / "check.c").write_text(f'#include "{BOARD / "keymaps/manna-harbour_miryoku/keymap.c"}"\n'
                                 f'static const uint8_t expected_dark[] = {{{expected}}};\n' + CHECK)
    subprocess.run(shlex.split(os.environ.get("CC", "cc")) +
                   ["-std=c11", "-Wall", "-Wextra", "-Werror", '-DQMK_KEYBOARD_H="stub.h"',
                    "-I", str(temp), str(temp / "check.c"), "-o", str(temp / "check")], check=True)
    subprocess.run([str(temp / "check")], check=True)
