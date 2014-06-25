#include "../vendor/ceedling/vendor/unity/src/unity.h"
#include "powermate_hid.h"

PowermateHid *test_hid;

PowermateData test_data = {
	.button_state = POWERMATE_OFF,
	.knob_displacement = 0,
	._unused = 0,
	.led_brightness = 0x80,
	.led_status = 0x14,
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

void test_get_input(void)
{
	test_error = powermate_hid_get_input(test_hid);

	TEST_ASSERT_EQUAL(POWERMATE_HID_SUCCESS, test_error);
	TEST_ASSERT_EQUAL_MEMORY(
		&test_data, &test_hid->last_input, 6);
}

void test_send_output(void)
{
	test_error = powermate_hid_set_control(test_hid, powermate_control_fast_pulse);

	TEST_ASSERT_EQUAL(POWERMATE_HID_SUCCESS, test_error);

	test_error = powermate_hid_send_output(test_hid);

	TEST_ASSERT_EQUAL(POWERMATE_HID_SUCCESS, test_error);

	TEST_ASSERT_EQUAL_MEMORY(
		&powermate_control_fast_pulse, &test_hid->control, sizeof(PowermateControl));
}