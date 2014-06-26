#include "powermate_hid.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hidapi.h"
#include "libusb.h"

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

void on_output_done(struct libusb_transfer *transfer);

libusb_device **dev_list = NULL;
libusb_context *dev_context = NULL;

int is_knob(libusb_device *dev);

PowermateError powermate_hid_send_output(PowermateHid *this)
{
  libusb_device  *temp_dev, *knob_device = NULL;
  libusb_device_handle *knob_handle = NULL;
  libusb_init(&dev_context);

  int dev_count = libusb_get_device_list(dev_context, &dev_list);
  int i, error;

  for(i = 0; i < dev_count; i++)
  {
    temp_dev = dev_list[i];
    if(is_knob(temp_dev))
    {
        knob_device = temp_dev;
        break;
    }
  }

  if(knob_device == NULL)
  {
  	libusb_free_device_list(dev_list, 1);
  	return POWERMATE_HID_ERROR_NOT_FOUND;
  }

  error = libusb_open(knob_device, &knob_handle);

  if(error || knob_handle == NULL)
  {
  	libusb_free_device_list(dev_list, 1);
  	return POWERMATE_HID_ERROR_ACCESS;
  }

  struct libusb_transfer *output_transfer = libusb_alloc_transfer(0);

  libusb_fill_control_setup(
    this->_internals->buffer,
    LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_INTERFACE |
    LIBUSB_ENDPOINT_OUT,
    1,
    this->control.upper_value << 8 | this->control.type,
    this->control.index,
    0);

  libusb_fill_control_transfer(
    output_transfer,
    knob_handle,
    this->_internals->buffer,
    on_output_done,
    NULL,
    1000);

  error = libusb_submit_transfer(output_transfer);

  if(error)
  {
  	libusb_close(knob_handle);
  	libusb_free_device_list(dev_list, 1);
  	libusb_free_transfer(output_transfer);
  	libusb_exit(dev_context);
  	dev_list = NULL;
  	dev_context = NULL;
  	return POWERMATE_HID_ERROR_UNKNOWN;
  }

  this->is_busy = 1;

  return POWERMATE_HID_SUCCESS;
}

int is_knob(libusb_device *dev)
{
    struct libusb_device_descriptor desc;

    libusb_get_device_descriptor(dev, &desc);

    return desc.idVendor  == POWERMATE_VID &&
           desc.idProduct == POWERMATE_PID;
}

void on_output_done(struct libusb_transfer *transfer)
{
	if(transfer->status != LIBUSB_SUCCESS)
	{
		printf("Control transfer failed: %d\n", transfer->status);
	}

	PowermateHid *this = (PowermateHid *)transfer->user_data;
    libusb_close(transfer->dev_handle);
    libusb_free_device_list(dev_list, 1);
	libusb_free_transfer(transfer);
	dev_list = NULL;
	libusb_exit(dev_context);
	dev_context = NULL;
	this->is_busy = 0;
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
