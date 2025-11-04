/*
 * Copyright (c) 2017 Linaro Limited
 * Copyright (c) 2018 Intel Corporation
 * Copyright (c) 2024 TOKITA Hiroshi
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/util.h>

#include "led_strip.h"

#define STRIP_NODE		DT_ALIAS(led_strip)

// #if DT_NODE_HAS_PROP(DT_ALIAS(led_strip), chain_length)
#define STRIP_NUM_PIXELS	DT_PROP(DT_ALIAS(led_strip), chain_length)
// #else
// #error Unable to determine length of LED strip
// #endif

#define DELAY_TIME K_MSEC(50)

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static const struct led_rgb colors[] = {
	RGB(0x0F, 0x00, 0x00), /* red */
	RGB(0x00, 0x0F, 0x00), /* green */
	RGB(0x00, 0x00, 0x0F), /* blue */
};

static struct led_rgb pixels[STRIP_NUM_PIXELS];

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

void led_strip_update(int r, int g, int b)
{
    for (size_t i = 0; i < STRIP_NUM_PIXELS; i++) {
        pixels[i].r = r;
        pixels[i].g = g;
        pixels[i].b = b;
    }
    led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
}

int led_strip_init(void)
{
	size_t color = 0;
	int rc;

	if (device_is_ready(strip)) {
		printk("Found LED strip device %s", strip->name);
	} else {
		printk("LED strip device %s is not ready", strip->name);
		return 0;
	}

	printk("Displaying pattern on strip");
	// while (1) {
	// 	for (size_t cursor = 0; cursor < ARRAY_SIZE(pixels); cursor++) {
	// 		memset(&pixels, 0x00, sizeof(pixels));
	// 		memcpy(&pixels[cursor], &colors[color], sizeof(struct led_rgb));

	// 		rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
	// 		if (rc) {
	// 			LOG_ERR("couldn't update strip: %d", rc);
	// 		}

	// 		k_sleep(DELAY_TIME);
	// 	}

	// 	color = (color + 1) % ARRAY_SIZE(colors);
	// }

	return 1;
}