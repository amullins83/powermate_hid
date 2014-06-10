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

typedef struct PowermateData
{
  uint8_t   button_state;
  int8_t    knob_displacement;
  uint8_t   _unused;
  uint8_t   led_brightness;
  LEDStatus led_status;
  uint8_t   led_multiplier;
} PowermateData;

typedef enum _powermate_hid_error
{
  POWERMATE_HID_SUCCESS = 0,
  POWERMATE_HID_ERROR_ACCESS,
  POWERMATE_HID_ERROR_NOT_FOUND,
  POWERMATE_HID_ERROR_TIMEOUT,
  POWERMATE_HID_ERROR_UNKNOWN
} PowermateError;

typedef struct _POWERMATE_HID
{
	// Lookup Capabilities and Reports
} PowermateHid;

PowermateHid *powermate_hid_new(void);
void powermate_hid_delete(PowermateHid *);

// These return 0 if all is well, otherwise a PowermateError code
PowermateError powermate_hid_get_input(PowermateHid *this);
PowermateError powermate_hid_send_output(PowermateHid *this);

#endif // powermate_hid_H
