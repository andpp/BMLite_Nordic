/**
 * @file   bmlite.h
 * @brief  BM-Lite HAL functions.
 * 
 *    All functions must be implemented in order to support BM-Lite on a Board
 */

#ifndef BMLITE_H
#define BMLITE_H

#include "fpc_bep_types.h"
#include "platform.h"

/*
 * @brief Board initialization
 * @param[in] SPI CLK speed
 */

void hal_board_init(uint32_t speed_hz);

/*
 * @brief Control BM-Lite Reset pin
 * @param[in] True  - Activate RESET
 *            False - Deactivate RESET
 */
void hal_bmlite_reset(bool state);

/*
 * @brief Init SPI interface
 * @param[in] SPI CLK speed
 */
void hal_bmlite_spi_init(uint32_t speed_hz);

/*
 * @brief SPI write-read
 * @param[in] Write buffer
 * @param[in] Read buffer
 * @param[in] Size
 * @param[in] Leave CS asserted
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t hal_bmlite_spi_write_read(uint8_t *write, uint8_t *read, size_t size,
        bool leave_cs_asserted);

/*
 * @brief Check if BM-Lite IRQ pin is set
 * @return ::bool
 */
bool hal_bmlite_get_status(void);

/**
 * @brief Initializes timebase. Starts system tick counter.
 */
void hal_timebase_init(void);

/**
 * @brief Reads the system tick counter.
 *
 * @return Tick count since hal_timebase_init() call. [ms]
 */
uint32_t hal_timebase_get_tick(void);

/**
 * @brief Busy wait.
 *
 * @param[in] ms  Time to wait [ms].
 * 0 => return immediately
 * 1 => wait at least 1ms etc.
 */
void hal_timebase_busy_wait(uint32_t ms);

/*
 *  Optional functions for Buttons & Leds control
 */

/*
 * @brief Get button press time (msec)
 * @return ::uint32_t
 */
uint32_t hal_get_button_press_time();

/*
 * @brief Set LED(s) status
 * @param[in] Status
 * @param[in] Status modifier
 */
void hal_set_leds(platform_led_status_t status, uint16_t mode);


#endif /* BMLITE_H */
