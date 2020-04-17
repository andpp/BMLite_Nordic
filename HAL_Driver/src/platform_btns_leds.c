/*
 * Copyright (c) 2019 Fingerprint Cards AB <tech@fingerprints.com>
 *
 * All rights are reserved.
 * Proprietary and confidential.
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Any use is subject to an appropriate license granted by Fingerprint Cards AB.
 */

/**
 * @file   btns_leds.c
 * @brief  Sensor control functions.
 */

#include "boards.h"
#include "btns_leds.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/** LED ON time in ms */
#define LED_SOLID_ON_TIME_MS 700

/** LED blink time in ms */
#define LED_BLINK_TIME_MS    200

/** Board button used for app control */
#define BMLITE_BUTTON 3


static void _board_set_leds(uint8_t color)
{
	uint32_t i;

	for(i=0; i<4; i++) {
		if (color & 1) {
             bsp_board_led_on(i);
        } else {
             bsp_board_led_off(i);
        }
        color = color >> 1;
	}
}

void board_set_led(platform_led_status_t color)
{
    _board_set_leds(color);
}
