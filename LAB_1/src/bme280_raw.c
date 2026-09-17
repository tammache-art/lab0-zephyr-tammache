#include <errno.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#include "bme280_raw.h"

LOG_MODULE_REGISTER(bme280_raw, LOG_LEVEL_INF);

#define BME280_CHIP_ID_REG       0xD0
#define BME280_CHIP_ID_VALUE     0x60
#define BME280_CALIB_TEMP_REG    0x88
#define BME280_CTRL_MEAS_REG     0xF4
#define BME280_TEMP_MSB_REG      0xFA

/*
 * Temperature oversampling x1:
 * osrs_t = 001
 *
 * Pressure measurement skipped:
 * osrs_p = 000
 *
 * Normal operating mode:
 * mode = 11
 */
#define BME280_CTRL_MEAS_VALUE   0x23

static int bme280_read_registers(const struct i2c_dt_spec *sensor,
                                 uint8_t start_register,
                                 uint8_t *data,
                                 size_t length)
{
        return i2c_burst_read_dt(sensor, start_register, data, length);
}

static int bme280_write_register(const struct i2c_dt_spec *sensor,
                                 uint8_t reg,
                                 uint8_t value)
{
        uint8_t message[] = {reg, value};

        return i2c_write_dt(sensor, message, sizeof(message));
}

int bme280_raw_init(const struct i2c_dt_spec *sensor,
                    struct bme280_calibration *calibration)
{
        uint8_t chip_id;
        uint8_t calibration_data[6];
        int error;

        if (!device_is_ready(sensor->bus)) {
                LOG_ERR("I2C controller is not ready");
                return -ENODEV;
        }

        error = bme280_read_registers(sensor, BME280_CHIP_ID_REG,
                                     &chip_id, sizeof(chip_id));
        if (error != 0) {
                LOG_ERR("Failed to read BME280 chip ID: %d", error);
                return error;
        }

        LOG_INF("BME280 chip ID: 0x%02x", chip_id);

        if (chip_id != BME280_CHIP_ID_VALUE) {
                LOG_ERR("Unexpected chip ID, expected 0x60");
                return -ENODEV;
        }

        error = bme280_read_registers(sensor, BME280_CALIB_TEMP_REG,
                                     calibration_data,
                                     sizeof(calibration_data));
        if (error != 0) {
                LOG_ERR("Failed to read calibration data: %d", error);
                return error;
        }

        calibration->dig_t1 =
                (uint16_t)((calibration_data[1] << 8) |
                           calibration_data[0]);

        calibration->dig_t2 =
                (int16_t)((calibration_data[3] << 8) |
                          calibration_data[2]);

        calibration->dig_t3 =
                (int16_t)((calibration_data[5] << 8) |
                          calibration_data[4]);

        LOG_INF("Calibration: T1=%u T2=%d T3=%d",
                calibration->dig_t1,
                calibration->dig_t2,
                calibration->dig_t3);

        error = bme280_write_register(sensor, BME280_CTRL_MEAS_REG,
                                      BME280_CTRL_MEAS_VALUE);
        if (error != 0) {
                LOG_ERR("Failed to configure CTRL_MEAS: %d", error);
                return error;
        }

        return 0;
}

int32_t bme280_compensate_temperature(
        int32_t adc_temperature,
        const struct bme280_calibration *calibration)
{
        int32_t var1;
        int32_t var2;
        int32_t t_fine;

        var1 = (((adc_temperature >> 3) -
                ((int32_t)calibration->dig_t1 << 1)) *
                (int32_t)calibration->dig_t2) >> 11;

        var2 = (((((adc_temperature >> 4) -
                  (int32_t)calibration->dig_t1) *
                 ((adc_temperature >> 4) -
                  (int32_t)calibration->dig_t1)) >> 12) *
                (int32_t)calibration->dig_t3) >> 14;

        t_fine = var1 + var2;

        /* Result is temperature in hundredths of a degree Celsius. */
        return (t_fine * 5 + 128) >> 8;
}

int bme280_raw_read_temperature(const struct i2c_dt_spec *sensor,
                                const struct bme280_calibration *calibration,
                                int32_t *temperature_centi_c)
{
        uint8_t temperature_data[3];
        int32_t adc_temperature;
        int error;

        error = bme280_read_registers(sensor, BME280_TEMP_MSB_REG,
                                     temperature_data,
                                     sizeof(temperature_data));
        if (error != 0) {
                LOG_ERR("Failed to read temperature registers: %d", error);
                return error;
        }

        adc_temperature =
                ((int32_t)temperature_data[0] << 12) |
                ((int32_t)temperature_data[1] << 4) |
                ((int32_t)temperature_data[2] >> 4);

        *temperature_centi_c =
                bme280_compensate_temperature(adc_temperature, calibration);

        return 0;
}