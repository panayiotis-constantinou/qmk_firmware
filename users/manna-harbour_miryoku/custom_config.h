// Copyright 2019 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

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
#define AG_PROMPT       QK_USER_21
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
    U_NU, AG_CLEAR, AG_REJECT, AG_PROMPT, U_NU, AG_AGENT_PICKER, AG_PANE_LEFT, AG_PANE_DOWN, AG_PANE_UP, AG_PANE_RIGHT, \
    AG_SCRATCHPAD, AG_HUNK, AG_LAZYGIT, AG_PALETTE, U_NU, AG_PANE_ZOOM, AG_WS_PREV, AG_TAB_PREV, AG_TAB_NEXT, AG_WS_NEXT, \
    U_NP, U_NP, LT(U_MEDIA, KC_ESC), AG_AGENT_PREV, AG_AGENT_NEXT, AG_ACCEPT, AG_AGENT_URGENT, LT(U_FUN, KC_DEL), U_NP, U_NP

#define MIRYOKU_LAYERMAPPING_AGENT MIRYOKU_MAPPING

#endif

