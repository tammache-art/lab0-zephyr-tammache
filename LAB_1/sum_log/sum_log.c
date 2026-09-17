#include <zephyr/logging/log.h>

#include "sum_log.h"

LOG_MODULE_REGISTER(sum_module);

int sum_log(int a, int b)
{
	int inputs[] = {a, b};
	int result;

	LOG_INF("Starting sum calculation");

	LOG_HEXDUMP_INF(inputs, sizeof(inputs), "Input values");

	if ((a < 0) || (b < 0)) {
		LOG_WRN("At least one input is negative");
	}

	result = a + b;

	LOG_INF("%d + %d = %d", a, b, result);

	return result;
}