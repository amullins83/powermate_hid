#include "powermate_hid.h"
#include <stdlib.h>
#include <string.h>
#include "hidapi.h"

#define POWERMATE_CONTROL_REQUEST_LENGTH 5
#define POWERMATE_INPUT_LENGTH 8

struct powermate_hid_internals {
	hid_device    *handle;
	unsigned char *buffer;
};

PowermateInternals *internals_new(void);
void internals_delete(PowermateInternals *this);

PowermateHid *powermate_hid_new(void)
{
	PowermateHid *powermate = malloc(sizeof *powermate);

	if(powermate != NULL)
	{
		powermate->_internals = internals_new();
		if(powermate->_internals != NULL)
		{
			return powermate;
		}
	}

	return NULL;
}

void powermate_hid_delete(PowermateHid *this)
{
	internals_delete(this->_internals);
	free(this);
}

// These return 0 if all is well, otherwise a powermate_hid_error code
PowermateError powermate_hid_get_input(PowermateHid *this)
{
	int read_bytes = hid_get_feature_report(
		this->_internals->handle,
		this->_internals->buffer,
		8);
    
    if(read_bytes < 0)
    {
    	return POWERMATE_HID_ERROR_ACCESS;
    }
    else if(read_bytes < 6)
    {
    	return POWERMATE_HID_ERROR_TIMEOUT;
    }
    else
    {
    	this->last_input = (PowermateData){
    		.button_state = this->_internals->buffer[0],
    		.knob_displacement = this->_internals->buffer[1],
    		._unused = 0,
    		.led_brightness = this->_internals->buffer[3],
    		.led_status = this->_internals->buffer[4],
    		.led_multiplier = this->_internals->buffer[5]
    	};

    	return POWERMATE_HID_SUCCESS;
    }
}

PowermateError powermate_hid_send_output(PowermateHid *this)
{
	this->_internals->buffer[0] = 0;

	this->_internals->buffer[1] = this->control.type;
	this->_internals->buffer[2] = this->control.upper_value;
	this->_internals->buffer[3] = this->control.index & 0xff;
	this->_internals->buffer[4] = (this->control.index >> 8) & 0xff;

	int write_bytes = hid_send_feature_report(
		this->_internals->handle,
		this->_internals->buffer,
		POWERMATE_CONTROL_REQUEST_LENGTH);

	if(write_bytes < 0)
	{
		return POWERMATE_HID_ERROR_ACCESS;
	}
	if(write_bytes < POWERMATE_CONTROL_REQUEST_LENGTH)
	{
		return POWERMATE_HID_ERROR_TIMEOUT;
	}
	else
	{
		return POWERMATE_HID_SUCCESS;
	}
}

PowermateError powermate_hid_set_control(PowermateHid *this, PowermateControl control)
{
	this->control = control;

	// I could do some validation here
	return POWERMATE_HID_SUCCESS;
}

PowermateInternals *internals_new(void)
{
	PowermateInternals *this = malloc(sizeof *this);

	if(this != NULL)
	{
		this->handle = hid_open(POWERMATE_VID, POWERMATE_PID, 0);
		if(this->handle != NULL)
		{
			this->buffer = malloc(POWERMATE_INPUT_LENGTH);
			
			if(this->buffer == NULL)
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}

	return this;
}

void internals_delete(PowermateInternals *this)
{
	free(this->buffer);
	hid_close(this->handle);
	free(this);
}


PowermateControl powermate_control_fast_pulse = {
	.type = POWERMATE_CONTROL_PULSE_MODE,
	.upper_value = POWERMATE_PULSE_TABLE_NORMAL,
	.index = 0x30 << 8 | POWERMATE_PULSE_SPEED_FAST
};

PowermateControl powermate_control_slow_pulse = {
    .type = POWERMATE_CONTROL_PULSE_MODE,
	.upper_value = POWERMATE_PULSE_TABLE_NORMAL,
	.index = 0x80 << 8 | POWERMATE_PULSE_SPEED_SLOW
};

PowermateControl powermate_control_led_bright = {
	.type = POWERMATE_CONTROL_STATIC_BRIGHTNESS,
	.upper_value = 0,
	.index = 0xff
};

PowermateControl powermate_control_led_dim = {
	.type = POWERMATE_CONTROL_STATIC_BRIGHTNESS,
	.upper_value = 0,
	.index = 0x80
};

PowermateControl powermate_control_led_off = {
	.type = POWERMATE_CONTROL_STATIC_BRIGHTNESS,
	.upper_value = 0,
	.index = POWERMATE_OFF
};

PowermateControl powermate_control_pulse_awake_on = {
	.type = POWERMATE_CONTROL_PULSE_AWAKE,
	.upper_value = 0,
	.index = POWERMATE_ON
};

PowermateControl powermate_control_pulse_awake_off = {
	.type = POWERMATE_CONTROL_PULSE_AWAKE,
	.upper_value = 0,
	.index = POWERMATE_OFF
};

PowermateControl powermate_control_pulse_asleep_on = {
	.type = POWERMATE_CONTROL_PULSE_ASLEEP,
	.upper_value = 0,
	.index = POWERMATE_ON
};

PowermateControl powermate_control_pulse_asleep_off = {
	.type = POWERMATE_CONTROL_PULSE_ASLEEP,
	.upper_value = 0,
	.index = POWERMATE_OFF
};
