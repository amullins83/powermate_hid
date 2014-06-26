#include "../vendor/ceedling/vendor/unity/src/unity.h"
#include "powermate_hid.h"

PowermateHid *test_hid;

PowermateData test_data = {
	.button_state = POWERMATE_OFF,
	.knob_displacement = 0,
	._unused = 0,
	.led_brightness = 0x80,
	.led_status = 0x00,
	.led_multiplier = 1
};

PowermateError test_error = POWERMATE_HID_SUCCESS;

void setUp(void)
{
	test_hid = powermate_hid_new();
}

void tearDown(void)
{
	if(test_hid != NULL)
	{
		powermate_hid_delete(test_hid);
		test_hid = NULL;
	}
}

void test_new(void)
{
	TEST_ASSERT_NOT_NULL(test_hid);
}

void send_control(PowermateControl control);

void test_get_input(void)
{
	send_control(powermate_control_slow_pulse);
	send_control(powermate_control_led_dim);

	test_error = powermate_hid_get_input(test_hid);

	TEST_ASSERT_EQUAL(POWERMATE_HID_SUCCESS, test_error);
	TEST_ASSERT_EQUAL_MEMORY(
		&test_data, &test_hid->last_input, 6);
}

void send_control(PowermateControl control)
{
	test_error = powermate_hid_set_control(test_hid, control);

	TEST_ASSERT_EQUAL(POWERMATE_HID_SUCCESS, test_error);

	test_error = powermate_hid_send_output(test_hid);

	TEST_ASSERT_EQUAL(POWERMATE_HID_SUCCESS, test_error);

	TEST_ASSERT_EQUAL_MEMORY(
		&control, &test_hid->control, sizeof(PowermateControl));
}

void test_send_pulse_awake_on(void)
{
	send_control(powermate_control_pulse_awake_on);
}

void test_send_fast_pulse(void)
{
	send_control(powermate_control_fast_pulse);
}

void test_send_slow_pulse(void)
{
	send_control(powermate_control_slow_pulse);
}

void test_send_led_off(void)
{
	send_control(powermate_control_led_off);
}

void test_send_led_on(void)
{
 	send_control(powermate_control_led_dim);
}

void test_send_led_bright(void)
{
	send_control(powermate_control_led_bright);
}

void test_send_pulse_awake_off(void)
{
	send_control(powermate_control_pulse_awake_off);
}

void test_send_pulse_asleep_on(void)
{
	send_control(powermate_control_pulse_asleep_on);
}

void test_send_pulse_asleep_off(void)
{
	send_control(powermate_control_pulse_asleep_off);
}
