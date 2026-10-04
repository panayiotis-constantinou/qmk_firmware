// Mouse Jiggler Header
// Copyright 2025 ZSA Technology Labs, Inc <@zsa>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "quantum.h"

// QK_USER_0 is RGB_LAYER_COLORS; keep this slot clear of the Herdr keys too.
enum custom_keycodes { MJ_TOGG = QK_USER_18 };

// Public API
bool mouse_jiggler_is_enabled(void);
void mouse_jiggler_enable(void);
void mouse_jiggler_disable(void);
void mouse_jiggler_toggle(void);

// Hooks
bool process_record_mouse_jiggler(uint16_t keycode, keyrecord_t *record);
void housekeeping_task_mouse_jiggler(void);
