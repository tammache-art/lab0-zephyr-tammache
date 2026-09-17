#include <stdint.h>

#include <zephyr/devicetree.h>
#include <zephyr/ztest.h>

#include "bme280_raw.h"

#define BME280_TEST_NODE DT_NODELABEL(bme280)

/*
 * Bosch datasheet reference values:
 * Raw temperature 519888 should produce 25.08 degrees Celsius.
 */
static const struct bme280_calibration reference_calibration = {
        .dig_t1 = 27504,
        .dig_t2 = 26435,
        .dig_t3 = -1000,
};

ZTEST(bme280_test_suite, test_devicetree_node)
{
        zassert_true(DT_NODE_EXISTS(BME280_TEST_NODE),
                     "BME280 node does not exist");

        zassert_true(DT_NODE_HAS_STATUS(BME280_TEST_NODE, okay),
                     "BME280 node is not enabled");

        zassert_equal(DT_REG_ADDR(BME280_TEST_NODE), 0x77,
                      "Expected BME280 address 0x77");
}

ZTEST(bme280_test_suite, test_reference_temperature)
{
        int32_t temperature;

        temperature = bme280_compensate_temperature(
                519888, &reference_calibration);

        zassert_equal(temperature, 2508,
                      "Expected Bosch reference temperature of 25.08 C");
}

ZTEST(bme280_test_suite, test_temperature_sanity_range)
{
        int32_t temperature;

        temperature = bme280_compensate_temperature(
                519888, &reference_calibration);

        zassert_true((temperature >= -4000) &&
                     (temperature <= 8500),
                     "Temperature is outside BME280 operating range");
}

ZTEST(bme280_test_suite, test_temperature_increases)
{
        int32_t lower_temperature;
        int32_t higher_temperature;

        lower_temperature = bme280_compensate_temperature(
                500000, &reference_calibration);

        higher_temperature = bme280_compensate_temperature(
                520000, &reference_calibration);

        zassert_true(higher_temperature > lower_temperature,
                     "Higher raw reading should produce higher temperature");
}

ZTEST_SUITE(bme280_test_suite, NULL, NULL, NULL, NULL, NULL);
