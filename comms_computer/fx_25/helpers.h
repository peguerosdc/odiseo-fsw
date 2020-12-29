/*
 * helpers.h
 *
 *  Created on: 10/10/2017
 *      Author: alponced
 */

#ifndef FX_25_HELPERS_H_
#define FX_25_HELPERS_H_

#include <stdint.h>
#include <stdlib.h>


/**
 * @brief Find the correlation tag of a series of bytes
 * @param[in] rxByte is the next byte received
 * @return 1 on success, 0 otherwise
 *
 * Finds the correlation tag defined as CORRELATION_TAG accumulating a series of bytes
 * received as rxByte
 */
uint8_t FindCorrelationTag(uint8_t rxByte);

/****************** Convolutional Code/Decoder functions  ***********************/

/**
 * @brief Function to get the XOR operation between the current bit of the 'byte_to_check'
 *        and the 'pos' bit of the same received byte according to 'poly'
**/
uint8_t shiftXOR(uint8_t poly, uint8_t memory, uint8_t pos, uint8_t curr_result);

/**
* @brief Function to insert into memory one bit of a register 'stream'
*         Starting from MSB until LSB
**/
uint8_t insertMemory(uint8_t stream, uint8_t memory);

/**
* @brief Function to group in one variable the encoded bits
**/
uint32_t orderEncoded(uint8_t results[4], uint32_t encoded);

/**
* @brief Convolutional 1/4 encoding of a byte with fixed polynomials with K=8
**/
uint32_t encodeByte(uint8_t byte_to_encode, uint8_t init);

/**
* @brief Return possible netx memory (state).
**/
uint8_t get_possible_memory(uint8_t inserBit, uint8_t memory);

/**
 * @brief Get the next possible result given a memory
 **/
uint8_t encode_to_possible_results(uint8_t possible_memory, uint8_t possible_results);

/**
* @brief Get the next two possible branches of the Viterbi (4 bits per node)
**/
uint8_t get_two_possibilities(uint8_t current_memory);

/**
 * @brief Get the hamming distance between 4 bits (LSB in bytes)
 * @param[in] rxByte is the next byte received
 * @return 1 on success, 0 otherwise
**/
uint8_t get_hamming_distance(uint8_t received_4bits, uint8_t possible_results);

uint8_t decide_0_or_1(uint8_t possibilities, uint8_t four_bits, uint8_t memory);

uint8_t return_2_decoded_bits(uint8_t received_byte, uint8_t current_memory);

uint8_t decodeByte(uint32_t encoded, uint8_t init);

uint8_t reverse(uint8_t b);

#endif /* FX_25_HELPERS_H_ */
