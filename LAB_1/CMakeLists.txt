#
# Copyright (c) 2025 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sum_application)

target_sources(app PRIVATE src/main.c)

target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/sum_printk
    ${CMAKE_CURRENT_SOURCE_DIR}/sum_log
)

target_sources_ifdef(CONFIG_SUM_PRINT app PRIVATE
    sum_printk/sum_printk.c
)

target_sources_ifdef(CONFIG_SUM_LOG app PRIVATE
    sum_log/sum_log.c
)