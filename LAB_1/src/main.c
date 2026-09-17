/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define POLL_TIME_MS 50

#define LED5180_NODE DT_ALIAS(led5180)
#define BUTTON_NODE DT_ALIAS(button5180)

static const struct gpio_dt_spec led =
	GPIO_DT_SPEC_GET(LED5180_NODE, gpios);

static const struct gpio_dt_spec button =
	GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

int main(void)
{
	int ret;
	bool led_state = false;
	bool previous_button_state = false;

	if (!gpio_is_ready_dt(&led)) {
		printk("LED device is not ready\n");
		return 0;
	}

	if (!gpio_is_ready_dt(&button)) {
		printk("Button device is not ready\n");
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		printk("Could not configure LED: %d\n", ret);
		return 0;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret < 0) {
		printk("Could not configure button: %d\n", ret);
		return 0;
	}

	printk("Press Button 1 to toggle LED2\n");

	while (1) {
		int button_state = gpio_pin_get_dt(&button);

		if (button_state < 0) {
			printk("Could not read button: %d\n", button_state);
			return 0;
		}

		if (button_state && !previous_button_state) {
			led_state = !led_state;

			ret = gpio_pin_set_dt(&led, led_state);
			if (ret < 0) {
				printk("Could not set LED: %d\n", ret);
				return 0;
			}

			printk("LED2 state: %s\n",
			       led_state ? "ON" : "OFF");
		}

		previous_button_state = button_state;
		k_msleep(POLL_TIME_MS);
	}

	return 0;
}