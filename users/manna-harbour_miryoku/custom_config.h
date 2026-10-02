// Copyright 2019 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

// The vendored Miryoku layers (QMK's last in-tree copy) still use keycode
// names QMK has since removed.
#define KC_BTN1 MS_BTN1
#define KC_BTN2 MS_BTN2
#define KC_BTN3 MS_BTN3
#define KC_MS_L MS_LEFT
#define KC_MS_D MS_DOWN
#define KC_MS_U MS_UP
#define KC_MS_R MS_RGHT
#define KC_WH_L MS_WHLL
#define KC_WH_D MS_WHLD
#define KC_WH_U MS_WHLU
#define KC_WH_R MS_WHLR
#define RGB_TOG RM_TOGG
#define RGB_MOD RM_NEXT
#define RGB_HUI RM_HUEU
#define RGB_SAI RM_SATU
#define RGB_VAI RM_VALU

// Personal Nav, Mouse, and Num layers in place of Miryoku's defaults; the
// FLIP, INVERTEDT, and VI alternatives stay Miryoku's.
#if !defined (MIRYOKU_LAYERS_FLIP)
  #if !defined (MIRYOKU_NAV_INVERTEDT) && !defined (MIRYOKU_NAV_VI)

// Caps Word on the left index finger's top key, Caps Lock below it on the
// inner home key, and Mission Control on the left pinky's bottom key.
#define MIRYOKU_LAYER_NAV \
    TD(U_TD_BOOT), TD(U_TD_U_TAP), TD(U_TD_U_EXTRA), CW_TOGG, U_NA, \
    U_RDO, U_PST, U_CPY, U_CUT, U_UND, \
    KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, KC_CAPS, \
    CW_TOGG, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, \
    KC_MCTL, KC_ALGR, TD(U_TD_U_NUM), TD(U_TD_U_NAV), U_NA, \
    KC_INS, KC_HOME, KC_PGDN, KC_PGUP, KC_END, \
    U_NP, U_NP, U_NA, U_NA, U_NA, KC_ENT, KC_BSPC, KC_DEL, U_NP, U_NP

// Cut, copy, and paste on the left top row in place of the layer tap dances;
// mouse movement as an inverted T on the right, with undo and redo either side
// of up; the jiggler toggle on the right inner bottom key; and a double-tap
// bootloader on the Space thumb.
#define MIRYOKU_LAYER_MOUSE \
    TD(U_TD_BOOT), U_CUT, U_CPY, U_PST, U_NA, \
    U_NU, U_UND, MS_UP, U_RDO, U_NU, \
    KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, U_NA, \
    U_NU, MS_LEFT, MS_DOWN, MS_RGHT, U_NU, \
    U_NA, KC_ALGR, TD(U_TD_U_SYM), TD(U_TD_U_MOUSE), U_NA, \
    MJ_TOGG, MS_WHLL, MS_WHLD, MS_WHLU, MS_WHLR, \
    U_NP, U_NP, U_NA, TD(U_TD_BOOT), U_NA, MS_BTN2, MS_BTN1, MS_BTN3, U_NP, U_NP

  #endif

// Calculator operators on the right top row in place of the layer tap dances.
#define MIRYOKU_LAYER_NUM \
    KC_LBRC, KC_7, KC_8, KC_9, KC_RBRC, \
    U_NA, KC_PLUS, KC_MINS, KC_ASTR, KC_SLSH, \
    KC_SCLN, KC_4, KC_5, KC_6, KC_EQL, \
    U_NA, KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI, \
    KC_GRV, KC_1, KC_2, KC_3, KC_BSLS, \
    U_NA, TD(U_TD_U_NUM), TD(U_TD_U_NAV), KC_ALGR, U_NA, \
    U_NP, U_NP, KC_DOT, KC_0, KC_MINS, U_NA, U_NA, U_NA, U_NP, U_NP

#endif

// Miryoku's Media layer puts OU_AUTO (USB/Bluetooth output selection) on the
// bottom row; these boards are USB only, so it cycles the OS mode for the
// clipboard keys instead: auto-detect, Mac, Linux.
#define U_OS_CYCLE OU_AUTO

#ifdef RGB_MATRIX_ENABLE

// Layer colors only show in solid color mode.
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR

#define RGB_LAYER_COLORS QK_USER_0

// Media layer with LED controls in place of Miryoku's RGB keys: B restores the
// layer colors after an effect; J toggles the LEDs; L/U cycle effects; Y/'
// adjust their speed.
#define MIRYOKU_LAYER_MEDIA \
    TD(U_TD_BOOT), TD(U_TD_U_TAP), TD(U_TD_U_EXTRA), TD(U_TD_U_BASE), RGB_LAYER_COLORS, \
    RM_TOGG, RM_NEXT, RM_PREV, RM_SPDU, RM_SPDD, \
    KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, U_NA, \
    U_NU, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT, \
    U_NA, KC_ALGR, TD(U_TD_U_FUN), TD(U_TD_U_MEDIA), U_NA, \
    U_OS_CYCLE, U_NU, U_NU, U_NU, U_NU, \
    U_NP, U_NP, U_NA, U_NA, U_NA, KC_MSTP, KC_MPLY, KC_MUTE, U_NP, U_NP

#endif

#ifdef HERDR_AGENT_ENABLE

#define AG_WS_PREV      QK_USER_1
#define AG_WS_NEXT      QK_USER_2
#define AG_TAB_PREV     QK_USER_3
#define AG_TAB_NEXT     QK_USER_4
#define AG_PANE_LEFT    QK_USER_5
#define AG_PANE_DOWN    QK_USER_6
#define AG_PANE_UP      QK_USER_7
#define AG_PANE_RIGHT   QK_USER_8
#define AG_AGENT_PICKER QK_USER_9
#define AG_SCRATCHPAD   QK_USER_10
#define AG_WS_NEW       QK_USER_11
#define AG_TAB_NEW      QK_USER_12
#define AG_LAZYGIT      QK_USER_13
#define AG_PALETTE      QK_USER_14
#define AG_PANE_ZOOM    QK_USER_15
#define AG_HUNK         QK_USER_16
#define AG_AGENT_NEXT   QK_USER_17
#define AG_ACCEPT       QK_USER_19
#define AG_REJECT       QK_USER_20
#define AG_CLEAR        QK_USER_22
#define AG_AGENT_PREV   QK_USER_23
#define AG_AGENT_URGENT QK_USER_24
#define AG_PANE_CLOSE   QK_USER_25
#define AG_TAB_CLOSE    QK_USER_26
#define AG_WS_CLOSE     QK_USER_27
#define AG_PANE_SPLIT   QK_USER_28
#define AG_SORT_TOGGLE  QK_USER_29
#define AG_SOUND_TOGGLE QK_USER_30

// Append the common agent control layer to every enabled Miryoku keymap.
#define MIRYOKU_LAYER_LIST \
MIRYOKU_X(BASE,   "Base") \
MIRYOKU_X(EXTRA,  "Extra") \
MIRYOKU_X(TAP,    "Tap") \
MIRYOKU_X(BUTTON, "Button") \
MIRYOKU_X(NAV,    "Nav") \
MIRYOKU_X(MOUSE,  "Mouse") \
MIRYOKU_X(MEDIA,  "Media") \
MIRYOKU_X(NUM,    "Num") \
MIRYOKU_X(SYM,    "Sym") \
MIRYOKU_X(FUN,    "Fun") \
MIRYOKU_X(AGENT,  "Herdr")

// The right hand says where, like the Nav layer: pane arrows on home, and
// workspace/tab steps below like Home/PgDn/PgUp/End.  The left hand says what:
// closing on top (mirroring the right's new keys), agent replies on home,
// tools below.  Thumbs cycle agents on the left and Accept sits where Enter is.
#define MIRYOKU_LAYER_AGENT \
    AG_SORT_TOGGLE, AG_PANE_CLOSE, AG_TAB_CLOSE, AG_WS_CLOSE, U_NU, U_NU, AG_WS_NEW, AG_TAB_NEW, AG_PANE_SPLIT, AG_SOUND_TOGGLE, \
    U_NU, AG_CLEAR, AG_REJECT, U_NU, U_NU, AG_AGENT_PICKER, AG_PANE_LEFT, AG_PANE_DOWN, AG_PANE_UP, AG_PANE_RIGHT, \
    AG_SCRATCHPAD, AG_HUNK, AG_LAZYGIT, AG_PALETTE, U_NU, AG_PANE_ZOOM, AG_WS_PREV, AG_TAB_PREV, AG_TAB_NEXT, AG_WS_NEXT, \
    U_NP, U_NP, LT(U_MEDIA, KC_ESC), AG_AGENT_PREV, AG_AGENT_NEXT, AG_ACCEPT, AG_AGENT_URGENT, LT(U_FUN, KC_DEL), U_NP, U_NP

#define MIRYOKU_LAYERMAPPING_AGENT MIRYOKU_MAPPING

#endif

