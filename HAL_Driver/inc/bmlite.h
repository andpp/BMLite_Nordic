/**
 * @file   bmlite.h
 * @brief  BM-Lite control functions.
 */

#ifndef BMLITE_H
#define BMLITE_H

bool nordic_bmlite_get_status(void);
void nordic_board_init(uint32_t speed_hz);
void nordic_bmlite_reset(bool state);

#endif /* BMLITE_H */
