/*
 * Copyright (c) 2019 Fingerprint Cards AB <tech@fingerprints.com>
 *
 * All rights are reserved.
 * Proprietary and confidential.
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Any use is subject to an appropriate license granted by Fingerprint Cards AB.
 */

/**
 * @file   sensor.c
 * @brief  Sensor control functions.
 */

#include <string.h>
#include <stdbool.h>

#include "nrf_gpio.h"
#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#define NRF_LOG_MODULE_NAME APP
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_gpiote.h"
#include "nrf_drv_gpiote.h"

#include "platform_spi.h"
// #include "fpc_timebase.h"

#define FPC_PIN_RESET   	ARDUINO_2_PIN
#define FPC_PIN_INTERRUPT   ARDUINO_A2_PIN

static volatile bool sensor_interrupt = false;

static void nordic_bmlite_gpio_init    (void);

void nordic_board_init(uint32_t speed_hz)
{

	if (NRF_UICR->REGOUT0 != UICR_REGOUT0_VOUT_3V3)
	{
	    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
	    while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
	    NRF_UICR->REGOUT0 = UICR_REGOUT0_VOUT_3V3;

	    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
	    while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
	}
	NRF_USBD->ENABLE = 1;

    nordic_bmlite_gpio_init();
    nordic_bmlite_spi_init(speed_hz);
}

void nordic_bmlite_reset(bool state)
{
    if(!state) {
	    nrf_drv_gpiote_out_set(FPC_PIN_RESET);
    } else {
        nrf_drv_gpiote_out_clear(FPC_PIN_RESET);
    }
}

bool nordic_bmlite_get_status(void)
{
    return nrf_drv_gpiote_in_is_set(FPC_PIN_INTERRUPT);
}

static void nordic_bmlite_gpio_init(void)
{
    ret_code_t err_code;
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(true);
    err_code = nrf_drv_gpiote_out_init(FPC_PIN_RESET, &out_config);
    APP_ERROR_CHECK(err_code);
	nrf_drv_gpiote_out_task_enable(FPC_PIN_RESET); //Enable task for output pin (toggle)

    nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    err_code = nrf_drv_gpiote_in_init(FPC_PIN_INTERRUPT, &config, NULL);
    APP_ERROR_CHECK(err_code);
    nrf_gpiote_event_clear(NRF_GPIOTE_EVENTS_IN_0);
    return;
}

