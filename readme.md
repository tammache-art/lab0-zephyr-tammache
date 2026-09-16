# ESE5180: Lab 0 Zephyr

| Team Member Name | Email Address                                                           |
| ---------------- | ----------------------------------------------------------------------- |
| Talal Ammache    | [tammache@engineering.upenn.edu](mailto:tammache@engineering.upenn.edu) |

**GitHub Repository URL:** https://github.com/tammache-art/lab0-zephyr-tammache

## 3. Building with West

### West Build

The application was built from the nRF Connect SDK terminal using:

```bash
west build --build-dir /Users/talalammache/lab0-zephyr-tammache/LAB_1/build-west /Users/talalammache/lab0-zephyr-tammache/LAB_1 --pristine --board nrf7002dk/nrf5340/cpuapp/ns --sysbuild -DBOARD_ROOT=/Users/talalammache/lab0-zephyr-tammache/LAB_1
```

![Successful West build](images/west-build.png)

### Core West Commands

* `west init`: Initializes a West workspace and obtains its manifest configuration.
* `west update`: Downloads or updates the projects listed in the workspace manifest to their specified revisions.
* `west build`: Configures and compiles a Zephyr application using CMake and Ninja.
* `west flash`: Programs a previously built Zephyr image onto the selected hardware.

### Build Command Arguments

| Argument                                         | Meaning                                                                        |
| ------------------------------------------------ | ------------------------------------------------------------------------------ |
| `west build`                                     | Invokes the West build command.                                                |
| `--build-dir .../LAB_1/build-west`               | Sets the directory where generated build files and compiled output are stored. |
| `/Users/talalammache/lab0-zephyr-tammache/LAB_1` | Specifies the application source directory.                                    |
| `--pristine`                                     | Deletes any previous build state before configuring and compiling.             |
| `--board nrf7002dk/nrf5340/cpuapp/ns`            | Targets the non-secure application core of the nRF5340 on the nRF7002 DK.      |
| `--sysbuild`                                     | Enables Zephyr’s multi-image system build process.                             |
| `-DBOARD_ROOT=.../LAB_1`                         | Passes the application directory to CMake as an additional board-search root.  |

### West Flash

The application was flashed to the nRF7002 DK using:

```bash
west flash -d /Users/talalammache/lab0-zephyr-tammache/LAB_1/build-direct --dev-id 1050755774 --erase
```

| Argument                    | Meaning                                                            |
| --------------------------- | ------------------------------------------------------------------ |
| `west flash`                | Invokes the West flash command.                                    |
| `-d .../LAB_1/build-direct` | Selects the build directory containing the compiled firmware.      |
| `--dev-id 1050755774`       | Selects the specific connected nRF7002 DK debug probe.             |
| `--erase`                   | Erases the device’s flash memory before programming the new image. |

The firmware was successfully programmed and verified on the nRF7002 DK.

![Successful West flash](images/west-flash.png)

### Why Zephyr Wraps CMake with West

Zephyr uses West to provide a consistent workspace-aware interface around CMake and Ninja. West understands the Zephyr manifest, board targets, modules, build directories, and flashing/debugging runners. This allows the same workflow to configure, compile, flash, and debug applications across many supported boards without manually managing each underlying tool.

## 4. Kconfig

Kconfig controls the software configuration of a Zephyr application. The application’s `prj.conf` currently explicitly enables GPIO support:

```text
CONFIG_GPIO=y
```

After building, the resolved configuration was inspected in `LAB_1/build-direct/LAB_1/zephyr/.config`. The build enabled the following relevant symbols:

```text
CONFIG_SERIAL=y
CONFIG_GPIO=y
CONFIG_CONSOLE=y
CONFIG_UART_CONSOLE=y
CONFIG_PRINTK=y
```

### Log Levels

| Value | Level   | Purpose                                  |
| ----- | ------- | ---------------------------------------- |
| `0`   | None    | Disables logging.                        |
| `1`   | Error   | Reports critical errors.                 |
| `2`   | Warning | Reports potentially harmful conditions.  |
| `3`   | Info    | Reports general application information. |
| `4`   | Debug   | Reports detailed debugging information.  |

### `prj.conf` and `menuconfig`

`prj.conf` contains the configuration values requested by the application. `menuconfig` provides an interactive interface for viewing Kconfig symbols, their dependencies, and their current values. Changes made through `menuconfig` affect the generated configuration in the build directory but should be added to `prj.conf` when they need to be preserved in the application source.

### Verifying Configuration Symbols

After building, the final resolved Kconfig symbols can be inspected in the generated `zephyr/.config` file inside the build directory. This verification is important because board defaults, dependencies, and conflicting options can change or reject values requested in `prj.conf`.
