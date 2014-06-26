#ifndef powermate_hid_H
#define powermate_hid_H
#include <stdint.h>

#define POWERMATE_VID 0x77d
#define POWERMATE_PID 0X410
#define POWERMATE_IN_ENDPOINT 0X81
#define POWERMATE_OUT_ENDPOINT 2
#define POWERMATE_IN_SIZE 6

typedef union
{
  uint8_t status;
  struct {
    unsigned pulse_enabled      : 1;
    unsigned                    : 1;
    unsigned pulse_while_asleep : 1;
    unsigned                    : 1;
    unsigned pulse_speed        : 2;
    unsigned pulse_style        : 2; 
  } bits;
} LEDStatus;

typedef struct powermate_hid_data
{
  uint8_t   button_state;
  int8_t    knob_displacement;
  uint8_t   _unused;
  uint8_t   led_brightness;
  LEDStatus led_status;
  uint8_t   led_multiplier;
} PowermateData;

typedef enum powermate_hid_error
{
  POWERMATE_HID_SUCCESS = 0,
  POWERMATE_HID_ERROR_ACCESS,
  POWERMATE_HID_ERROR_NOT_FOUND,
  POWERMATE_HID_ERROR_TIMEOUT,
  POWERMATE_HID_ERROR_UNKNOWN
} PowermateError;

typedef enum powermate_hid_control_type
{
  POWERMATE_CONTROL_STATIC_BRIGHTNESS  = 1,
  POWERMATE_CONTROL_PULSE_ASLEEP       = 2,
  POWERMATE_CONTROL_PULSE_AWAKE        = 3,
  POWERMATE_CONTROL_PULSE_MODE         = 4,
  POWERMATE_HID_CONTROL_UNKNOWN
} PowermateControlType;

typedef enum powermate_hid_control_pulse_table
{
  POWERMATE_PULSE_TABLE_NORMAL = 0,
  POWERMATE_PULSE_TABLE_WEIRD  = 1,
  POWERMATE_PULSE_TABLE_CRAZY  = 2
} PowermatePulseTable;

typedef enum powermate_hid_control_pulse_speed
{
  POWERMATE_PULSE_SPEED_SLOW   = 0,
  POWERMATE_PULSE_SPEED_NORMAL = 1,
  POWERMATE_PULSE_SPEED_FAST   = 2
} PowermatePulseSpeed;

typedef enum powermate_hid_control_switch
{
  POWERMATE_OFF = 0,
  POWERMATE_ON  = 1
} PowermateSwitch;

typedef struct powermate_hid_pulse_index
{
  unsigned led_multiplier : 2;
  unsigned                : 6;
  unsigned speed          : 8;
} PowermatePulseIndex;

typedef struct powermate_hid_control
{
  PowermateControlType type;
  uint8_t  upper_value;
  uint16_t index;
} PowermateControl;

// Predefined control outputs
extern PowermateControl powermate_control_fast_pulse;
extern PowermateControl powermate_control_slow_pulse;

extern PowermateControl powermate_control_led_bright;
extern PowermateControl powermate_control_led_dim;
extern PowermateControl powermate_control_led_off;

extern PowermateControl powermate_control_pulse_awake_on;
extern PowermateControl powermate_control_pulse_awake_off;

extern PowermateControl powermate_control_pulse_asleep_on;
extern PowermateControl powermate_control_pulse_asleep_off;

typedef struct powermate_hid_internals PowermateInternals;

typedef struct powermate_hid
{
	PowermateData       last_input;
  PowermateControl    control;
  PowermateError      last_error;
  char                is_busy;
  PowermateInternals *_internals;
} PowermateHid;

PowermateHid *powermate_hid_new(void);
void powermate_hid_delete(PowermateHid *this);

typedef void (*PowermateCallback)(PowermateHid *this);

// These return 0 if all is well, otherwise a PowermateError code
// The callback parameter, if not NULL, is called when the transfer
// completes.
PowermateError powermate_hid_get_input(PowermateHid *this);

PowermateError powermate_hid_set_control(
  PowermateHid *this,
  PowermateControl control);

PowermateError powermate_hid_send_output(PowermateHid *this);

#endif // powermate_hid_H
