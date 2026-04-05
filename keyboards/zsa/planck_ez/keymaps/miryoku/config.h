#pragma once

#ifdef AUDIO_ENABLE
#define STARTUP_SONG SONG(PLANCK_SOUND)
#endif

#define MIDI_BASIC

#define ENCODER_RESOLUTION 4

#define QUICK_TAP_TERM 0

#define LAYER_LOCK_IDLE_TIMEOUT 3000
#undef RGB_MATRIX_TIMEOUT
#define RGB_MATRIX_TIMEOUT 900000

#define USB_SUSPEND_WAKEUP_DELAY 0
#undef MOUSEKEY_INTERVAL
#define MOUSEKEY_INTERVAL 14

#undef MOUSEKEY_MAX_SPEED
#define MOUSEKEY_MAX_SPEED 11

#undef MOUSEKEY_WHEEL_INTERVAL
#define MOUSEKEY_WHEEL_INTERVAL 85

#define SERIAL_NUMBER "DY9WP/m5Ejj3"
#define LAYER_STATE_16BIT
#define HSS(report) host_system_send(record->event.pressed ? report : 0); return false
#define HCS(report) host_consumer_send(record->event.pressed ? report : 0); return false

#define RGB_MATRIX_STARTUP_SPD 60
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR

