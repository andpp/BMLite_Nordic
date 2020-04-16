/**
 * @file    platform_spi.h
 * @brief   Platform SPI functions.
 */

#ifndef PLATFORM_SPI_H
#define PLATFORM_SPI_H

#include "fpc_bep_types.h"

fpc_bep_result_t nordic_bmlite_spi_write_read(uint8_t *write, uint8_t *read, size_t size,
        bool leave_cs_asserted);

void nordic_bmlite_spi_init(uint32_t speed_hz);

#endif /* PLATFORM_SPI_H */
