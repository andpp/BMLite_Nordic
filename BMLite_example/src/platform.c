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

/**
 * @file    platform.c
 * @brief   Platform specific functions
 */


#include "fpc_bep_types.h"
#include "platform.h"
#include "bmlite_hal.h"

bool platform_init(uint32_t speed_hz)
{
    hal_board_init(4000000);
    hal_timebase_init();

    platform_bmlite_reset();
    return true;
}

void platform_bmlite_reset(void)
{
    hal_bmlite_reset(true);
    hal_timebase_busy_wait(100);
    hal_bmlite_reset(false);
    hal_timebase_busy_wait(100);
}

fpc_bep_result_t platform_bmlite_send(uint16_t size, const uint8_t *data, uint32_t timeout,
        void *session)
{
    uint8_t buff[size];

    return hal_bmlite_spi_write_read((uint8_t *)data, buff, size, false);
}

fpc_bep_result_t platform_bmlite_receive(uint16_t size, uint8_t *data, uint32_t timeout,
        void *session)
{
	volatile uint32_t start_time = hal_timebase_get_tick();
	volatile uint32_t curr_time = start_time;
    // Wait for BM_Lite Ready for timeout or indefinitely if timeout is 0
    while (!hal_bmlite_get_status() &&
    		(!timeout || (curr_time = hal_timebase_get_tick()) - start_time < timeout)) {
                if(platform_check_button_pressed()) {
                    return FPC_BEP_RESULT_TIMEOUT;
                }
    }
    if(timeout && curr_time - start_time >= timeout) {
        return FPC_BEP_RESULT_TIMEOUT;
    }

    uint8_t buff[size];
    return hal_bmlite_spi_write_read(buff, data, size, false);
}

