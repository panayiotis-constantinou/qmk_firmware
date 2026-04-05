#define QUICK_TAP_TERM 0

#undef RGB_MATRIX_TIMEOUT
#define RGB_MATRIX_TIMEOUT 600000

#define USB_SUSPEND_WAKEUP_DELAY 0
#undef MOUSEKEY_INTERVAL
#define MOUSEKEY_INTERVAL 12

#undef MOUSEKEY_MAX_SPEED
#define MOUSEKEY_MAX_SPEED 11

#undef MOUSEKEY_WHEEL_INTERVAL
#define MOUSEKEY_WHEEL_INTERVAL 85

#define SERIAL_NUMBER "9BAVo/ea5DVr"
#define LAYER_STATE_8BIT
#define HCS(report) host_consumer_send(record->event.pressed ? report : 0); return false

#define RGB_MATRIX_STARTUP_SPD 60

