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

/*
 * Modified by Andrey Perminov for running on microcontrollers
 */


/**
 * @file    main.c
 * @brief   Main file for FPC BM-Lite Communication example.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "bep_host_if.h"
#include "com_common.h"
#include "platform.h"
// #include "fpc_hal_interfaces.h"

void bmlite_on_error(bmlite_error_t error, int32_t value)
{
    if(value != FPC_BEP_RESULT_TIMEOUT) {
        platform_set_led(3);
        platform_timebase_busy_wait(500);
        platform_set_led(0);
        platform_timebase_busy_wait(500);
        platform_set_led(3);
        platform_timebase_busy_wait(500);
        platform_set_led(0);
        platform_timebase_busy_wait(500);
    } else {
        platform_set_led(3);
        platform_timebase_busy_wait(100);
        platform_set_led(0);
    }
}

// void bmlite_on_start_capture();
// void bmlite_on_finish_capture();

void bmlite_on_start_enroll()
{
    platform_set_led(1);
    platform_timebase_busy_wait(500);
    platform_set_led(2);
    platform_timebase_busy_wait(500);
}

void bmlite_on_finish_enroll()
{
    platform_set_led(1);
    platform_timebase_busy_wait(100);
    platform_set_led(0);
    platform_timebase_busy_wait(100);
    platform_set_led(1);
    platform_timebase_busy_wait(100);
    platform_set_led(0);
}

void bmlite_on_start_enrollcapture()
{
    platform_set_led(3);
}

void bmlite_on_finish_enrollcapture()
{
    platform_set_led(0);
}

void bmlite_on_identify_start()
{
    platform_set_led(0);
}

// void bmlite_on_identify_finish();


int main (int argc, char **argv)
{
    int baudrate = 4000000;
    uint8_t buffer[512];
    uint16_t size[2] = { 256, 256 };
    fpc_com_chain_t hcp_chain;

    if(!platform_init(baudrate)) {
        platform_halt_if_debug();
    }

    init_com_chain(&hcp_chain, buffer, size, NULL);
    hcp_chain.channel = 1;

    {
        char version[100];
        uint16_t template_id;
        uint32_t current_id = 0;
        bool match;

        memset(version, 0, 100);
        fpc_bep_result_t res = bep_version(&hcp_chain, version, 99);
        if (res == FPC_BEP_RESULT_OK)
        {
            printf("%s\n", version);
        }

        platform_set_led(BMLITE_LED_STATUS_READY);

        while (1)
        {
            uint32_t btn_time = platform_get_button_press_time();
            if (btn_time < 200) {
                // nothing hapened
            } else if (btn_time < 5000) {
                // Enroll
                res = bep_enroll_finger(&hcp_chain);
                res = bep_save_template(&hcp_chain, current_id++);
            } else {
                // Erase All templates
                platform_set_led(BMLITE_LED_STATUS_DELETE_TEMPLATES);
                platform_timebase_busy_wait(500);
                res = bep_delete_template(&hcp_chain, REMOVE_ID_ALL_TEMPLATES);
            }
            res = bep_identify_finger(&hcp_chain, &template_id, &match);
            if (res != FPC_BEP_RESULT_OK)
                continue;
            if (match) {
                platform_set_led(BMLITE_LED_STATUS_MATCH);
            } else {
                platform_set_led(BMLITE_LED_STATUS_NOMATCH);
            }
            platform_timebase_busy_wait(500);
        }
    }
}
