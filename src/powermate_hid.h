#ifndef powermate_hid_H
#define powermate_hid_H

typedef enum _powermate_hid_error
{
  POWERMATE_HID_SUCCESS = 0,
  POWERMATE_HID_ERROR_ACCESS,
  POWERMATE_HID_ERROR_NOT_FOUND,
  POWERMATE_HID_ERROR_TIMEOUT,
  POWERMATE_HID_ERROR_UNKNOWN
} powermate_hid_error;

typedef struct _POWERMATE_HID
{
	// Lookup Capabilities and Reports
} PowermateHid;

PowermateHid *powermate_hid_new(void);
void powermate_hid_delete(PowermateHid *);

// These return 0 if all is well, otherwise a powermate_hid_error code
int powermate_hid_get_input(PowermateHid *this);
int powermate_hid_send_output(PowermateHid *this);

#endif // powermate_hid_H
