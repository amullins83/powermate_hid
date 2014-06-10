#include "powermate_hid.h"
#include <stdlib.h>

PowermateHid *powermate_hid_new(void)
{
	return (PowermateHid *)malloc(sizeof(PowermateHid));
}

void powermate_hid_delete(PowermateHid *this)
{
	free(this);
}

// These return 0 if all is well, otherwise a powermate_hid_error code
int powermate_hid_get_input(PowermateHid *this)
{
	return POWERMATE_HID_ERROR_UNKNOWN;
}

int powermate_hid_send_output(PowermateHid *this)
{
	return POWERMATE_HID_ERROR_UNKNOWN;
}
