#include <zephyr/ztest.h>

#include "sum_log.h"

ZTEST(sum_log_test_suite, test_sum_log_basic)
{
	int result = sum_log(2, 3);

	zassert_equal(result, 5, "Expected 2 + 3 to equal 5");
}

ZTEST(sum_log_test_suite, test_sum_log_negative)
{
	int result = sum_log(-8, 3);

	zassert_equal(result, -5, "Expected -8 + 3 to equal -5");
}

ZTEST(sum_log_test_suite, test_sum_log_zero)
{
	int result = sum_log(0, 0);

	zassert_equal(result, 0, "Expected 0 + 0 to equal 0");
}

ZTEST_SUITE(sum_log_test_suite, NULL, NULL, NULL, NULL, NULL);