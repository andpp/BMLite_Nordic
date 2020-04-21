/*
 * Copyright (c) 2020 Fingerprint Cards AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

/**
 * @file    platform.h
 * @brief   Platform specific function interface
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "fpc_com_result.h"

/**
 * @brief LED status.
 *
 * Different LED status.
 */
typedef enum {
    BMLITE_LED_STATUS_READY = 0,
    BMLITE_LED_STATUS_MATCH,
    BMLITE_LED_STATUS_NOMATCH,
    BMLITE_LED_STATUS_WAITTOUCH,
    BMLITE_LED_STATUS_STARTENROLL,
    BMLITE_LED_STATUS_FINISHENROLL,
    BMLITE_LED_STATUS_DELETE_TEMPLATES
} platform_led_status_t;

/**
 * @brief Initializes board
 *
 * @param[in]       speed_hz    Baudrate.
 */
bool platform_init(uint32_t speed_hz);

/**
 * @brief Does BM-Lite HW Reset
 *
 */
void platform_bmlite_reset(void);

/**
 * @brief Sends data over communication port in blocking mode.
 *
 * @param[in]       size        Number of bytes to send.
 * @param[in]       data        Data buffer to send.
 * @param[in]       timeout     Timeout in ms. Use 0 for infinity.
 *
 * @return ::fpc_com_result_t
 */
fpc_com_result_t platform_bmlite_send(uint16_t size, const uint8_t *data, uint32_t timeout,
        void *session);

/**
 * @brief Receives data from communication port in blocking mode.
 *
 * @param[in]       size        Number of bytes to receive.
 * @param[in, out]  data        Data buffer to fill.
 * @param[in]       timeout     Timeout in ms. Use 0 for infinity.
 *
 * @return ::fpc_com_result_t
 */
fpc_com_result_t platform_bmlite_receive(uint16_t size, uint8_t *data, uint32_t timeout,
        void *session);

/**
 * @brief Initializes timebase. Starts system tick counter.
 */
void platform_timebase_init(void);

/**
 * @brief Reads the system tick counter.
 *
 * @return Tick count since platform_timebase_init() call. [ms]
 */
uint32_t platform_timebase_get_tick(void);

/**
 * @brief Busy wait.
 *
 * @param[in] ms  Time to wait [ms].
 * 0 => return immediately
 * 1 => wait at least 1ms etc.
 */
void platform_timebase_busy_wait(uint32_t ms);

/**
 * @brief Get time in micro seconds
 *
 * @return time in us.
 */
uint32_t platform_get_time(void);

/**
 * @brief Stops execution if a debug interface is attached.
 */
void platform_halt_if_debug(void);

/**
 * @brief Performs a software reset.
 */
void platform_sw_reset(void) __attribute__((__noreturn__));

/**
 * @brief Set LED to a given color and display status.
 *
 * @param[in] color Color of RGB LED.
 * @param[in] status Status of LED.
 */
void platform_set_led(platform_led_status_t color);

/**
 * @brief Get button press time.
 *
 * @return Button press time in milli seconds.
 */
uint32_t platform_get_button_press_time(void);

#endif /* PLATFORM_H */
