#include "powermate_hid.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libusb.h"
#include <time.h>
#include "hidapi.h"

struct powermate_hid_internals {
    libusb_context         *context;
    libusb_device         **dev_list;
    libusb_device          *device;   
    libusb_device_handle   *handle;
    struct libusb_transfer *input_transfer;
    struct libusb_transfer *control_transfer;
    struct timeval         *tv;
    int                    input_lock;
    int                    control_lock; 
    unsigned char *buffer;
    hid_device             *hid_device;
};

PowermateInternals *internals_new(void);
void internals_delete(PowermateInternals *this);

PowermateHid *powermate_hid_new(void)
{
    PowermateHid *powermate = malloc(sizeof *powermate);

    if(powermate == NULL)
    {
        printf("Failed to allocate PowermateHid\n");
        return NULL;
    }

    powermate->_internals = internals_new();
    

    if(powermate->_internals == NULL)
    {
        printf("Failed to create internals\n");
        free(powermate);
        return NULL;
    }

    return powermate;
}

void powermate_hid_delete(PowermateHid *this)
{
    internals_delete(this->_internals);
    free(this);
}

void on_input_done(PowermateHid *this);

// These return 0 if all is well, otherwise a powermate_hid_error code
PowermateError powermate_hid_get_input(PowermateHid *this)
{
    char bytes_read = 0;

    PowermateInternals *pmints = this->_internals;
    pmints->buffer[0] = POWERMATE_IN_ENDPOINT;

    bytes_read = hid_get_feature_report(pmints->hid_device, pmints->buffer, POWERMATE_IN_SIZE);
    
    if(bytes_read != POWERMATE_IN_SIZE)
    {
        this->last_error = POWERMATE_HID_ERROR_UNKNOWN;
    }
    else
    {
        on_input_done(this);
    }

    return this->last_error;
}

void on_input_done(PowermateHid *this)
{
    PowermateInternals *pmints = this->_internals;

    this->last_input = (PowermateData){
        .button_state = pmints->buffer[1],
        .knob_displacement = pmints->buffer[2],
        ._unused = 0,
        .led_brightness = pmints->buffer[4],
        .led_status = pmints->buffer[5],
        .led_multiplier = pmints->buffer[6]
    };

    this->last_error = POWERMATE_HID_SUCCESS;
}

void on_output_done(struct libusb_transfer *transfer);

int is_knob(libusb_device *dev);

PowermateError powermate_hid_send_output(PowermateHid *this)
{
    PowermateInternals *pmints = this->_internals;
    int error = POWERMATE_HID_SUCCESS;

    libusb_handle_events_timeout_completed(pmints->context,
        pmints->tv, &pmints->control_lock);

    pmints->control_lock = 0;

    libusb_fill_control_setup(
      pmints->buffer,
      LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_INTERFACE |
      LIBUSB_ENDPOINT_OUT,
      1,
      this->control.upper_value << 8 | this->control.type,
      this->control.index,
      0);

    libusb_fill_control_transfer(
      pmints->control_transfer,
      pmints->handle,
      pmints->buffer,
      on_output_done,
      this,
      1000);

    error = libusb_submit_transfer(pmints->control_transfer);

    if(error)
    {
      this->last_error = POWERMATE_HID_ERROR_UNKNOWN;
      pmints->control_lock = 1;
    }
    else
    {
      this->is_busy++;
      this->last_error = POWERMATE_HID_SUCCESS;
    }

    return this->last_error;
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
    PowermateHid *this = (PowermateHid *)transfer->user_data;

    if(transfer->status != LIBUSB_SUCCESS)
    {
        this->last_error = POWERMATE_HID_ERROR_TIMEOUT;
    }
    else
    {
        this->last_error = POWERMATE_HID_SUCCESS;
    }

    this->_internals->control_lock = 1;
    this->is_busy--;
}

PowermateError powermate_hid_set_control(PowermateHid *this, PowermateControl control)
{
    this->control = control;

    // I could do some validation here
    this->last_error = POWERMATE_HID_SUCCESS;
    return this->last_error;
}

void reset_tv(struct timeval *tv);

PowermateError powermate_hid_wait(PowermateHid *this)
{
    PowermateInternals *pmints = this->_internals;

    reset_tv(pmints->tv);

    libusb_handle_events_timeout_completed(
        pmints->context,
        pmints->tv,
        &pmints->control_lock);

    reset_tv(pmints->tv);

    libusb_handle_events_timeout_completed(
        pmints->context,
        pmints->tv,
        &pmints->input_lock);
}


void null_internals(PowermateInternals *this);

struct timeval tv_reset = {2, 0};


void reset_tv(struct timeval *tv)
{
    if(tv != NULL)
    {
        memcpy(tv, &tv_reset, sizeof tv_reset);
    }
}

PowermateInternals *internals_new(void)
{
    PowermateInternals *this = malloc(sizeof *this);
    int device_count = 0, i, error;
    libusb_device *temp = NULL;

    if(this == NULL)
    {
        printf("\tFailed to allocate internals\n");
        return NULL;
    }

    null_internals(this);

    error = libusb_init(&this->context);

    if(error)
    {
        printf("\tFailed to initialize context\n");
        internals_delete(this);
        return NULL;
    }

    device_count = libusb_get_device_list(this->context, &this->dev_list);
    if(this->dev_list == NULL)
    {
        printf("\tNo devices found\n");
        internals_delete(this);
        return;
    }


    this->device = NULL;
    
    for(i = 0; i < device_count; i++)
    {
        temp = this->dev_list[i];
        if(is_knob(temp))
        {
            this->device = temp;
            break;
        }
    }

    if(this->device == NULL)
    {
        printf("\tFailed to enumerate device\n");
        internals_delete(this);
        return NULL;
    }

    error += libusb_open(this->device, &this->handle);

    if(error)
    {
        printf("\tFailed to get handle\n");
        return NULL;
    }

    this->buffer = malloc(POWERMATE_IN_SIZE + 1);

    if(this->buffer == NULL)
    {
        printf("\tFailed to allocate buffer\n");
        return NULL;
    }

    this->input_transfer = libusb_alloc_transfer(0);
    this->control_transfer = libusb_alloc_transfer(0);

    if(error || this->input_transfer == NULL ||
        this->control_transfer == NULL)
    {
        printf("\tFailed to allocate transfers\n");
        internals_delete(this);
        return NULL;
    }

    this->tv = malloc(sizeof *this->tv);

    reset_tv(this->tv);

    this->hid_device = hid_open(POWERMATE_VID, POWERMATE_PID, NULL);

    return this;
}

void null_internals(PowermateInternals *this)
{
    this->buffer = NULL;
    this->handle = NULL;
    this->dev_list = NULL;
    this->input_transfer = NULL;
    this->control_transfer = NULL;
    this->context = NULL;
    this->tv = NULL;
    this->control_lock = 1;
    this->input_lock = 1;
    this->hid_device = NULL;
}

void internals_delete(PowermateInternals *this)
{
    reset_tv(this->tv);

    if(this->context != NULL && this->tv != NULL)
    {
        libusb_handle_events_timeout_completed(this->context,
            this->tv, &this->control_lock);
    }

    free(this->buffer);

    libusb_close(this->handle);
    
    libusb_free_device_list(this->dev_list, 1);
    libusb_free_transfer(this->control_transfer);
    libusb_free_transfer(this->input_transfer);
    libusb_exit(this->context);
    free(this->tv);
    hid_close(this->hid_device);
    null_internals(this);

    free(this);
}


PowermateControl powermate_control_pulse_fast = {
    .type = POWERMATE_CONTROL_PULSE_MODE,
    .upper_value = POWERMATE_PULSE_TABLE_NORMAL,
    .index = 0x30 << 8 | POWERMATE_PULSE_SPEED_FAST
};

PowermateControl powermate_control_pulse_slow = {
    .type = POWERMATE_CONTROL_PULSE_MODE,
    .upper_value = POWERMATE_PULSE_TABLE_NORMAL,
    .index = 0x80 << 8 | POWERMATE_PULSE_SPEED_NORMAL
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
