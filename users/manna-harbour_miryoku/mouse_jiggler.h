// Mouse Jiggler Header
// Copyright 2025 ZSA Technology Labs, Inc <@zsa>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "quantum.h"

// Custom keycode for mouse jiggler toggle
enum custom_keycodes { MJ_TOGG = SAFE_RANGE };

// Public API
bool mouse_jiggler_is_enabled(void);
void mouse_jiggler_enable(void);
void mouse_jiggler_disable(void);
void mouse_jiggler_toggle(void);

// Hooks
bool process_record_mouse_jiggler(uint16_t keycode, keyrecord_t *record);
void housekeeping_task_mouse_jiggler(void);
