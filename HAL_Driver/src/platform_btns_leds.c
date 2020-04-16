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

static uint32_t btn_press_start;
static uint32_t btn_pressed;

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
    // if (status == FPC_HAL_LED_STATUS_OFF) {
    //     board_set_leds(0);
    // } else if (status == FPC_HAL_LED_STATUS_SOLID) {
    //     board_set_leds(color);
    //     platform_timebase_busy_wait(LED_SOLID_ON_TIME_MS);
    //     board_set_leds(0);
    // } else if (status == FPC_HAL_LED_STATUS_ON) {
    //     board_set_leds(color);
    // } else {
    //     uint32_t led_status = (uint32_t)(status - FPC_HAL_LED_STATUS_ONE_BLINK + 1);
    //     for (uint32_t i = 0; i < (led_status * 2); i++) {
    //         if (i % 2 == 0) {
    //             board_set_leds(0x8 + color);
    //         } else {
    //             board_set_leds(0x8);
    //         }
    //         platform_timebase_busy_wait(LED_BLINK_TIME_MS);
    //     }
    // }
}

uint32_t board_get_button_press_time()
{
    uint32_t time = 0;
    if (bsp_board_button_state_get(BMLITE_BUTTON)) {
        if (btn_pressed == 0) {
            btn_press_start = platform_timebase_get_tick();
            btn_pressed = 1;
        }
    } else { // Btn released
        if (btn_pressed) {
            uint32_t curr_time = platform_timebase_get_tick();
            if (curr_time > btn_press_start) {
                time = curr_time - btn_press_start;
            } else {
                time = curr_time + ~btn_press_start + 1;
            }
            btn_pressed = 0;
        }
    }
    return time;
}
