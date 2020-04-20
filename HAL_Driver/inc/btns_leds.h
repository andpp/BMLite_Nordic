/**
 * @file   btns_leds.h
 * @brief  Buttons and LEDs control functions
 */

#ifndef BTNS_LEDS_H
#define BTNS_LEDS_H

#include "platform.h"

void board_set_leds(platform_led_status_t color);
uint32_t board_get_button_press_time();

#endif
