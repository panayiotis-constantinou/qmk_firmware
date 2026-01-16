/* Copyright 2021 weteor
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// generated from users/manna-harbour_miryoku/miryoku.org  -*- buffer-read-only: t -*-

#include "mouse_jiggler.h"

// Hook into process_record_user to handle MJ_TOGG keycode
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_mouse_jiggler(keycode, record)) {
        return false;
    }
    return true;
}

// Hook into housekeeping_task_user to perform the jiggling
void housekeeping_task_user(void) {
    housekeeping_task_mouse_jiggler();
}
