/*
 * ESE5180 Lab 0
 * Section 8: Raw BME280 I2C temperature measurement
 */

#include <stdint.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "bme280_raw.h"

LOG_MODULE_REGISTER(bme280_app, LOG_LEVEL_INF);

#define BME280_NODE DT_NODELABEL(bme280)

#if !DT_NODE_HAS_STATUS(BME280_NODE, okay)
#error "BME280 Devicetree node is not enabled"
#endif

static const struct i2c_dt_spec bme280 =
        I2C_DT_SPEC_GET(BME280_NODE);

int main(void)
{
        struct bme280_calibration calibration;
        int32_t temperature;
        int32_t magnitude;
        int error;

        LOG_INF("Starting raw BME280 application");

        error = bme280_raw_init(&bme280, &calibration);
        if (error != 0) {
                LOG_ERR("BME280 initialization failed: %d", error);
                return 0;
        }

        LOG_INF("BME280 initialized successfully");

        while (1) {
                error = bme280_raw_read_temperature(
                        &bme280, &calibration, &temperature);

                if (error != 0) {
                        LOG_ERR("Temperature reading failed: %d", error);
                } else {
                        magnitude = temperature < 0 ?
                                    -temperature : temperature;

                        LOG_INF("Temperature: %s%d.%02d C",
                                temperature < 0 ? "-" : "",
                                magnitude / 100,
                                magnitude % 100);
                }

                k_sleep(K_SECONDS(2));
        }

        return 0;
}