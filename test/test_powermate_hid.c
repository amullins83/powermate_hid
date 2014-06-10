#include "unity.h"
#include "powermate_hid.h"

PowermateHid *test_hid;

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
