/*
 * helpers.c
 *
 *  Created on: 10/10/2017
 *      Author: alponced
 */

#include <fx_25/helpers.h>

/**
 * One correlation tag chosen from the FX.25 recommendation.
 * This tag is associated with no Reed-Solomon FEC
 */
const uint64_t CORRELATION_TAG = 0x93210201E8F4C706;

uint8_t FindCorrelationTag(uint8_t rxByte) {
    uint8_t* crawler = (uint8_t*)&CORRELATION_TAG;
    static uint8_t count = 0;
    // Look for the tag
    if( rxByte == *(crawler+count) ) {
        count++;
        if( count == 8){
            count = 0;
            return 1;
        }
    }
    else if( rxByte == *crawler ) {
        count = 1;
    }
    else {
        count = 0;
    }
    return 0;
}

/**
* Inserted polynoms:
* (R)LSB  (L)MSB
* 1010 0111
* 1011 1001
* 1101 1011
* 1111 1101
**/
const uint8_t POLYS[4] = {229, 157, 219, 191};

uint8_t shiftXOR(uint8_t poly, uint8_t memory, uint8_t pos, uint8_t curr_result){
    uint8_t xorPoly = poly;
    uint8_t xorByte = memory;
    if (pos > 0){
        xorPoly = poly << pos;
        xorByte = memory << pos;
    }
    uint8_t xor = xorPoly & 128;
    if (xor == 128) {
        if (curr_result == 255) {
            curr_result = xorByte & 128;
            return curr_result >> 7;
        } else {
            curr_result ^= ((xorByte & 128) >> 7);
            return curr_result;
        }
    } else {
        return curr_result;
    }
}

uint8_t insertMemory(uint8_t stream, uint8_t memory) {
    memory = memory >> 1;                                           // Moves actual memory to right
    uint8_t check_bit = stream & 128;
    if (check_bit == 128) {
        memory ^= (-1 ^ memory) & (1 << 7);
    } else {
        memory ^= (-0 ^ memory) & (1 << 7);
    }
    return memory;
}

uint32_t orderEncoded(uint8_t results[4], uint32_t encoded) {
    uint8_t i;
    uint32_t new_encoded = encoded << 4;                            // Move already encoded bits to make room to new ones
    for (i = 0; i < 4; i++) {                                       // Put new bits in order
        if (results[i] == 1) {
            new_encoded ^= (-1 ^ new_encoded) & (1 << (3 - i));
        } else {
            new_encoded ^= (-0 ^ new_encoded) & (1 << (3 - i));
        }
    }
    return new_encoded;
}

uint32_t encodeByte(uint8_t byte_to_encode, uint8_t init) {
    uint8_t result[4];                              // Array to store each result of that particular memory xored
    static uint8_t memory;                          // Memory (window) to store K=8. Static to keep past memory
    uint32_t encoded = 0;                           // Result to return
    uint8_t stream = byte_to_encode;                // Received stream
    uint8_t i, j, k;

    if (init == 1) {                                // To restore memory to initial state
        memory = 0;
    }

    for (i = 0; i < 8; i++) {                       // Iterate each bit in byte (stream)
        memory = insertMemory(stream, memory);      // Move corresponding bit to memory
        stream = stream << 1;                       // Move to next bit to encode
        for (k = 0; k < 4; k++) {                   // Return to initial states resulting bits
            result[k] = 255;
        }
        for (j = 0; j < 8; j++) {                   // Iterate over the whole memory (XOR bit per bit in memory)
            for (k = 0; k < 4; k++) {               // Code with the 4 specified polynomials
                result[k] = shiftXOR(POLYS[k], memory, j, result[k]);
            }
        }
        encoded = orderEncoded(result, encoded);    // Append to result last 4 resulting bits
    }
    return encoded;
}

uint8_t get_possible_memory(uint8_t insertBit, uint8_t memory) {
    memory = memory >> 1;                           // Moves actual memory to right

    if (insertBit == 1) {                           // Insert one or zero to the memory
        memory ^= (-1 ^ memory) & (1 << 7);
    }
    else {
        memory ^= (-0 ^ memory) & (1 << 7);
    }

    return memory;                                  // Return the result with new 0 or 1
}

uint8_t encode_to_possible_results(uint8_t possible_memory, uint8_t possible_results) {
    uint8_t result[4] = {255, 255, 255, 255};
    uint8_t i, j;

    for (i = 0; i < 8; i++) {                   // Iterate over the whole memory (XOR bit per bit in memory)
        for (j = 0; j < 4; j++) {               // Code with the 4 specified polynomials
            result[j] = shiftXOR(POLYS[j], possible_memory, i, result[j]);
        }
    }
    return orderEncoded(result, possible_results);  // Append result to last 4 possible bits in possible_results
}

uint8_t get_two_possibilities(uint8_t current_memory) {
    uint8_t possible_results = 0;

    uint8_t poss_memory1 = get_possible_memory((uint8_t)1, current_memory);
    uint8_t poss_memory0 = get_possible_memory((uint8_t)0, current_memory);

    possible_results = encode_to_possible_results(poss_memory0, possible_results);
    possible_results = encode_to_possible_results(poss_memory1, possible_results);

    return possible_results;
}

uint8_t get_hamming_distance(uint8_t received_4bits, uint8_t possible_result) {
    /* Hamming distance (weight) is the number of 1s resulting in an XOR operation */
    uint8_t to_count = received_4bits ^ possible_result;
    uint8_t counter = 0;
    uint8_t i;
    /* Iterate over the result of the XOR (4 bits) */
    for (i = 0; i < 4; i++) {
        // AND operation to check if the ith bit is 1
        if ((to_count & (uint8_t)1 << i) >> i == 1) {
            counter = counter + 1;
        }
    }
    return counter;
}

uint8_t decide_0_or_1(uint8_t possibilities, uint8_t four_bits, uint8_t memory) {
    uint8_t hamming0 = get_hamming_distance(((possibilities >> 4) & 15), four_bits);
    uint8_t hamming1 = get_hamming_distance((possibilities & 15), four_bits);
    if (hamming0 > hamming1) {
        return get_possible_memory((uint8_t)1, memory);
    } else if (hamming1 > hamming0) {
        return get_possible_memory((uint8_t)0, memory);
    } else {
        /* random int between 0 and 1*/
        int r = rand() % 2;
        if (r == 0){
            return get_possible_memory((uint8_t)0, memory);
        } else {
            return get_possible_memory((uint8_t)1, memory);
        }
    }
}

uint8_t return_2_decoded_bits(uint8_t received_byte, uint8_t current_memory) {
    uint8_t possibilities = 0;

    possibilities = get_two_possibilities(current_memory);
    current_memory = decide_0_or_1(possibilities, ((received_byte >> 4) & 15), current_memory);

    possibilities = get_two_possibilities(current_memory);
    current_memory = decide_0_or_1(possibilities, (received_byte & 15), current_memory);

    return current_memory;
}

uint8_t reverse(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

uint8_t decodeByte(uint32_t encoded, uint8_t init) {
    static uint8_t memory;
    uint8_t i;

    if (init == 1) {
        memory = 0;
    }

    for (i = 4; i > 0; i--) {
        memory = return_2_decoded_bits((uint8_t)(encoded >> 8 * (i - 1)), memory);
    }

    return reverse(memory);
}
