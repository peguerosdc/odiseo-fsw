/*
 * helpers.h
 *
 *  Created on: 10/10/2017
 */

#ifndef CRC_H_
#define CRC_H_

#include <stdint.h>

#define CRC_STATE_MACHINE_START         0
#define CRC_STATE_MACHINE_POST_START    1
#define CRC_STATE_MACHINE_RESET_CRC     2
#define CRC_STATE_MACHINE_NORMAL_OP     3

/* The real-time computed CRC of the frame that is received */
uint16_t crc;
/* The possible received CRC */
uint16_t candidateCRC;

/**
 * @brief Get CRC residue
 *
 * @param[in] AX.25 to check for errors
 * Results checked with: http://crccalc.com/
 * Init: 0x0000, refin: false, refout: false, xorout: 0x0000
 * Code inspired by: http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html#ch5
 */
uint16_t _residueCRC(uint8_t init, uint8_t byte_to_check);

void _pushToCRC(uint8_t initFlag, uint8_t rx_byte);

void compute_crc_of_byte(uint8_t state_machine, uint8_t rx_byte);

uint8_t is_crc_valid();

uint16_t get_crc();

#endif /* CRC_H_ */
