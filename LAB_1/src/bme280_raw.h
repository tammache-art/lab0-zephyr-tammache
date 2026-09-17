#ifndef BME280_RAW_H
#define BME280_RAW_H

#include <stdint.h>
#include <zephyr/drivers/i2c.h>

struct bme280_calibration {
        uint16_t dig_t1;
        int16_t dig_t2;
        int16_t dig_t3;
};

int bme280_raw_init(const struct i2c_dt_spec *sensor,
                    struct bme280_calibration *calibration);

int bme280_raw_read_temperature(const struct i2c_dt_spec *sensor,
                                const struct bme280_calibration *calibration,
                                int32_t *temperature_centi_c);

int32_t bme280_compensate_temperature(
        int32_t adc_temperature,
        const struct bme280_calibration *calibration);

#endif