# ESE5180: Lab 0 Zephyr

| Team Member Name | Email Address |
| --- | --- |
| Talal Ammache | [tammache@engineering.upenn.edu](mailto:tammache@engineering.upenn.edu) |

**GitHub Repository:** [https://github.com/tammache-art/lab0-zephyr-tammache](https://github.com/tammache-art/lab0-zephyr-tammache)

## 3. Building with West

### West Build

The application was built for the nRF5340 application core on the nRF7002 DK using:

```bash
west build -p always \
  -b nrf7002dk/nrf5340/cpuapp \
  -d LAB_1/build-direct \
  LAB_1
```

![Successful West build](images/west-build.png)

### Core West Commands

- `west init`: Initializes a West workspace and obtains its manifest configuration.
- `west update`: Downloads or updates the projects listed in the workspace manifest.
- `west build`: Configures and compiles a Zephyr application using CMake and Ninja.
- `west flash`: Programs a previously compiled Zephyr image onto the selected hardware.

### Build Command Arguments

| Argument | Meaning |
| --- | --- |
| `west build` | Invokes the West build command. |
| `-p always` | Creates a pristine build and removes previous generated build state. |
| `-b nrf7002dk/nrf5340/cpuapp` | Targets the nRF5340 application core on the nRF7002 DK. |
| `-d LAB_1/build-direct` | Selects the directory used for generated build files. |
| `LAB_1` | Specifies the application source directory. |

### West Flash

The application was flashed to the nRF7002 DK using:

```bash
west flash \
  -d LAB_1/build-direct \
  --dev-id 1050755774 \
  --erase
```

| Argument | Meaning |
| --- | --- |
| `west flash` | Invokes the West flash command. |
| `-d LAB_1/build-direct` | Selects the directory containing the compiled firmware. |
| `--dev-id 1050755774` | Selects the specific connected nRF7002 DK debug probe. |
| `--erase` | Erases the board's flash memory before programming the new image. |

The firmware was successfully programmed and verified on the nRF7002 DK.

![Successful West flash](images/west-flash.png)

### Why Zephyr Wraps CMake with West

Zephyr uses West to provide a workspace-aware interface around CMake and Ninja. West understands the Zephyr manifest, modules, board targets, build directories, and hardware runners. This allows developers to build, flash, and debug applications through a consistent workflow without manually configuring each underlying tool.

## 4. Kconfig

Kconfig controls which Zephyr subsystems and application features are compiled. Application configuration requests are stored in `prj.conf`.

The final resolved configuration is generated in:

```text
LAB_1/build-direct/LAB_1/zephyr/.config
```

The application used the following relevant configuration symbols:

```text
CONFIG_SERIAL=y
CONFIG_GPIO=y
CONFIG_CONSOLE=y
CONFIG_UART_CONSOLE=y
CONFIG_PRINTK=y
CONFIG_LOG=y
CONFIG_LOG_DEFAULT_LEVEL=3
```

### Log Levels

| Value | Level | Purpose |
| --- | --- | --- |
| `0` | None | Disables logging. |
| `1` | Error | Reports critical failures. |
| `2` | Warning | Reports potentially harmful conditions. |
| `3` | Info | Reports normal application information. |
| `4` | Debug | Reports detailed diagnostic information. |

### `prj.conf` and `menuconfig`

`prj.conf` contains the configuration values requested by the application and is stored with the source code.

`menuconfig` provides an interactive interface for examining symbols, dependencies, and resolved values. Changes made only through `menuconfig` affect the generated build configuration. Settings that should remain after a pristine build must be placed in `prj.conf`.

### Verifying Configuration Symbols

The generated `.config` file shows the final values after Zephyr processes board defaults and Kconfig dependencies. This is important because a requested value can be changed or disabled when its dependencies are not satisfied.

The relevant values can be checked using:

```bash
grep -E '^CONFIG_(SERIAL|GPIO|CONSOLE|UART_CONSOLE|PRINTK|LOG)=' \
  LAB_1/build-direct/LAB_1/zephyr/.config
```

## 5. Devicetree

### 5.1 LED Alias

The Devicetree overlay defines an application-specific alias named `led5180`:

```dts
/ {
        aliases {
                led5180 = &led1;
        };
};
```

The application accesses the LED through the alias:

```c
#define LED_NODE DT_ALIAS(led5180)
```

The `led1` Devicetree node corresponds to physical LED 2 on the nRF7002 DK. This allowed the application to select the required LED without directly referencing the board node in `main.c`.

### 5.2 Button-Controlled LED

The application was extended to poll a button and toggle the LED whenever a valid button press was detected.

A short polling interval was used to monitor the button. The application also tracked the previous button state so that the LED was toggled only on a new press, rather than continuously while the button was held.

### 5.3 Button Alias

A second application-specific alias was added for the button:

```dts
/ {
        aliases {
                led5180 = &led1;
                button5180 = &button0;
        };
};
```

The source code accesses the button through:

```c
#define BUTTON_NODE DT_ALIAS(button5180)
```

Using aliases keeps board-specific hardware information in the Devicetree overlay and keeps the application source independent from the board's original node names.

### Why Use a Devicetree Overlay?

A Devicetree overlay customizes the application's hardware description without modifying Zephyr's shared board DTS files.

This approach:

- Keeps the SDK's original board definitions unchanged.
- Stores application-specific hardware choices in the repository.
- Makes the application easier to move to another board.
- Prevents local SDK changes from being lost during an update.
- Separates hardware configuration from application behavior.

## 6. `printk` and Zephyr Logging

Two implementations of a sum function were created.

### 6.1 `printk` Implementation

The `sum_printk()` function calculates the sum and displays it using `printk()`:

```c
int sum_printk(int a, int b)
{
        int result = a + b;

        printk("printk: %d + %d = %d\n", a, b, result);

        return result;
}
```

For inputs `-5` and `12`, the application produced:

```text
printk: -5 + 12 = 7
```

### 6.2 Logging Implementation

The `sum_log()` implementation uses Zephyr's logging subsystem. It:

- Logs the beginning of the calculation at the information level.
- Produces a hexdump of the input values.
- Produces a warning when at least one input is negative.
- Logs the completed calculation.
- Returns the calculated sum.

Example output:

```text
<inf> sum_module: Starting sum calculation
<inf> sum_module: Input values
fb ff ff ff 0c 00 00 00
<wrn> sum_module: At least one input is negative
<inf> sum_module: -5 + 12 = 7
```

### 6.3 Kconfig Selection

A Kconfig choice allows one implementation to be selected at build time:

```kconfig
choice SUM_IMPLEMENTATION
        prompt "Sum output implementation"
        default SUM_PRINT

config SUM_PRINT
        bool "Use printk"

config SUM_LOG
        bool "Use Zephyr logging"

endchoice

source "Kconfig.zephyr"
```

The application selects the logging implementation in `prj.conf` using:

```text
CONFIG_SUM_LOG=y
```

CMake conditionally compiles only the selected source file:

```cmake
target_sources_ifdef(CONFIG_SUM_PRINT app PRIVATE
    sum_printk/sum_printk.c
)

target_sources_ifdef(CONFIG_SUM_LOG app PRIVATE
    sum_log/sum_log.c
)
```

### `printk` Compared with Zephyr Logging

| Feature | `printk` | Zephyr Logging |
| --- | --- | --- |
| Output | Immediate console text | Structured log messages |
| Severity levels | Not supported | Error, warning, info, and debug |
| Runtime filtering | Not supported | Supported |
| Module names | Not included | Included |
| Timestamps | Not included automatically | Supported |
| Hexdump support | Manual | Built in |
| Best use | Simple debugging | Larger and configurable applications |

### Why Source `Kconfig.zephyr`?

The application Kconfig file defines local application symbols. Sourcing `Kconfig.zephyr` includes Zephyr's main Kconfig tree so that standard kernel, driver, console, logging, and testing options remain available.

### Deferred Logging

With deferred logging, a logging call places a compact message in a buffer. Formatting and transmission happen later in a logging-processing context.

This reduces the amount of time spent inside time-sensitive application code. However, it requires buffer memory, and messages can be dropped if the buffer becomes full.

## 7. Ztest Unit Testing

A standalone Ztest application was created in:

```text
LAB_1/tests/SUM_UNIT_TEST
```

The test suite verifies three cases:

- `2 + 3 = 5`
- `-8 + 3 = -5`
- `0 + 0 = 0`

The test configuration enables Ztest, verbose assertions, and logging:

```text
CONFIG_ZTEST=y
CONFIG_ZTEST_ASSERT_VERBOSE=2
CONFIG_LOG=y
CONFIG_LOG_DEFAULT_LEVEL=3
```

The tests were built and executed in QEMU using:

```bash
cd LAB_1/tests/SUM_UNIT_TEST

west build -p always \
  -b qemu_cortex_m3 \
  . \
  --no-sysbuild

west build -t run
```

All three test cases passed:

```text
SUITE PASS - 100.00% [sum_log_test_suite]
pass = 3, fail = 0, skip = 0, total = 3

PROJECT EXECUTION SUCCESSFUL
```

### How Does Ztest Run Without a User `main()`?

When `CONFIG_ZTEST=y` is enabled, Zephyr links its Ztest runner into the application. The Ztest runner provides the application entry point.

The `ZTEST` and `ZTEST_SUITE` macros register the test cases during the build. After the Zephyr kernel starts, the framework discovers and executes the registered tests automatically.

### West Build and Twister Comparison

| Feature | `west build` and `west build -t run` | `west twister` |
| --- | --- | --- |
| Scope | Runs one configured application | Discovers scenarios from `testcase.yaml` |
| Platforms | Uses one selected board | Can test multiple platforms |
| Output | Direct build and Ztest console output | Aggregated test results and reports |
| Reports | Mainly terminal output | JSON and xUnit reports |
| Best use | Fast local development | Regression testing and continuous integration |

The sum test suite was also executed with Twister:

```bash
cd LAB_1

west twister \
  -T tests/SUM_UNIT_TEST \
  -p qemu_cortex_m3 \
  --inline-logs \
  -v
```

Twister reported that one of one configurations and three of three test cases passed.

## 8. BME280 Peripheral

### 8.1 Hardware Setup

The BME280 environmental sensor was connected to the nRF7002 DK using I2C. The sensor breakout is STEMMA QT compatible, and the completed setup used the following signals:

| BME280 Connection | nRF7002 DK Connection |
| --- | --- |
| `VIN` | `VDD` |
| `GND` | `GND` |
| `SCL` | `P1.14` |
| `SDA` | `P1.15` |

![nRF7002 DK connected to the BME280](images/8.1_hardware.jpg)

The BME280 was configured at I2C address `0x77`.

### 8.2 Raw I2C Implementation

The application communicates with the BME280 directly through Zephyr's I2C API rather than using Zephyr's built-in BME280 sensor driver.

The following configuration options were enabled in `prj.conf`:

```text
CONFIG_SENSOR=y
CONFIG_I2C=y
CONFIG_CBPRINTF_FP_SUPPORT=y
```

The BME280 was added to the Devicetree overlay:

```dts
&i2c1 {
        status = "okay";

        bme280: bme280@77 {
                compatible = "i2c-device";
                status = "okay";
                reg = <0x77>;
        };
};
```

The raw implementation performs the following operations:

1. Reads register `0xD0` and verifies that the chip ID is `0x60`.
2. Reads the temperature calibration values from registers `0x88` through `0x8D`.
3. Writes `0x23` to the `CTRL_MEAS` register at `0xF4`.
4. Burst-reads the raw temperature from registers `0xFA` through `0xFC`.
5. Combines the three register values into a 20-bit raw temperature value.
6. Applies the BME280 integer temperature-compensation formula.
7. Logs the calculated temperature every two seconds.

### 8.3 Hardware Results

The initial tests produced I2C errors `-116` and `-5`. These errors indicated that the sensor was not responding correctly on the I2C bus.

Correcting and reseating the sensor connections resolved the issue. The application then:

- Detected chip ID `0x60`.
- Read the temperature calibration values.
- Initialized the sensor successfully.
- Produced stable temperature measurements.

![BME280 temperature output](images/bme280-temperature.png)

The measured temperature was approximately `24.9 C`.

### 8.4 BME280 Unit Tests

A separate Ztest application was created in:

```text
LAB_1/tests/BME280_UNIT_TEST
```

The tests use known calibration coefficients and simulated raw sensor values. This allows the temperature-compensation logic to be tested without requiring the physical sensor.

The test suite verifies:

- The BME280 Devicetree node exists.
- The Devicetree node is enabled.
- The configured I2C address is `0x77`.
- The Bosch reference raw value produces `25.08 C`.
- The calculated temperature is inside the BME280 operating range.
- A larger raw value produces a larger compensated temperature.

The tests were executed directly in QEMU:

```bash
cd LAB_1/tests/BME280_UNIT_TEST

west build -p always \
  -b qemu_cortex_m3 \
  . \
  --no-sysbuild

west build -t run
```

All four tests passed successfully.

![BME280 QEMU Ztest results](images/bme280-ztest-qemu.png)

### 8.5 Twister Results

The test suite was also executed using Twister:

```bash
cd LAB_1

west twister \
  -T tests/BME280_UNIT_TEST \
  -p qemu_cortex_m3 \
  --inline-logs \
  -v
```

Twister reported:

```text
1 of 1 executed test configurations passed
4 of 4 executed test cases passed
0 failed
0 errored
0 warnings
```

![BME280 Twister results](images/bme280-ztest-twister.png)

### QEMU and Twister

`west build -t run` executes one configured test application directly in QEMU and displays its Ztest output.

Twister reads `testcase.yaml`, discovers test scenarios, builds and runs them for the selected platforms, and generates structured JSON and xUnit reports. Twister is therefore more suitable for automated regression testing and continuous integration.