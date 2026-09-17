cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(bme280_unit_test)

target_sources(app PRIVATE
    src/bme280_test.c
    ../../src/bme280_raw.c
)

target_include_directories(app PRIVATE
    ../../src
)