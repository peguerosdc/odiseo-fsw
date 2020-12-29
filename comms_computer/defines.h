/*
 * defines.h
 *
 *  Created on: 25/10/2017
 *      Author: peguerosdc
 */

#ifndef DEFINES_H_
#define DEFINES_H_

/******************************************************************
 * Includes
 ******************************************************************/
#ifdef DEBUGGING
    /*
     * For debugging in a PC, we need printf in stdio.h
     */
    #include <stdio.h>
#else
    /*
     * For building for the MSP430, we need the appropiate headers
     */
    #include <comms/comms.h>
    #include <msp430.h>
#endif

#include <radio_utils/radio_utils.h>
#include <fx_25/crc.h>
#include <fx_25/helpers.h>
#include <stdint.h>
#include <stddef.h>


/******************************************************************
 * Function Prototypes
 ******************************************************************/

/*
 * @brief Initializes peripherials and board configurations
 */
void initBoard(void);

/*
 * @brief State 1 of the State Machine.
 * param[in] rx_byte is the byte to be processed
 *
 * This state is in charge of looking for the start AX.25 flag
 */
void state1(uint8_t rx_byte);

/*
 * @brief State 2 of the State Machine.
 * param[in] rx_byte is the byte to be processed
 *
 * This state is in charge of reading the addresses on-the-fly and
 * find if the frame is inteded to be for this satellite and if so,
 * it stores de source address to reply.
 */
void state2(uint8_t rx_byte);
/* Counter to go through the addresses */
uint8_t addressCounter;
/* Destination address that must match the satellite's */
uint64_t destinationAddress;
/* Source address that will received this reply */
uint8_t sourceAddress[7];
uint8_t source_address_index;

/*
 * @brief State 3 of the State Machine.
 * param[in] rx_byte is the byte to be processed
 *
 * This state checks that the Control byte match the requirements
 * for an Information Frame
 */
void state3(uint8_t rx_byte);

/*
 * @brief State 4 of the State Machine.
 * param[in] rx_byte is the byte to be processed
 *
 * This state checks that the PID byte match the Layer 3
 * requirements.
 */
void state4(uint8_t rx_byte);

/*
 * @brief State 5 of the State Machine.
 * param[in] rx_byte is the byte to be processed
 *
 * This state stores the payload in the buffer until the CRC arrives.
 * After that, it checks the CRC is OK and if so, sends the frame
 * to the master CPU.
 */
void state5(uint8_t rx_byte);

/*
 * @brief triggers a pin interrupt on the master so it knows that there is data available
 */
void notify_master_data_available(void);

/*
 * @brief triggers a pin interrupt on the master so it knows that this uC is ready to transmit data
 */
void notify_master_transmission_ready(void);

/******************************************************************
 * Radio control methods
 ******************************************************************/

void put_radio_transmit_mode(void);

void put_radio_receive_mode(void);

/******************************************************************
 * Variables and constants
 ******************************************************************/
#ifdef DEBUGGING
    /* Test frame to be "received" */
        uint8_t TEST_VALUE_NOT_ENCODED[] = {
                // Correlation tag:
                0x06, 0xC7, 0xF4, 0xE8, 0x01, 0x02, 0x21, 0x93,
                // Init flag:
                0x7E,
                // Destination address must match satellite's:
                0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0xE0,
                // Send repeater's addresses (optional). The final bit must NOT be 1:
                // A maximum of 8 repeaters are allowed by AX25, but we dont care about this
                // as they are processed on the go
                0x1A, 0x1B, 0x1A, 0x1B, 0x1A, 0x1B, 0x12,
                // Final source address. Final bit is 1
                0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x01,
                // Control byte:
                0x00,
                // PID byte:
                0xF0,
                // Payload should be here
                0x01, 0x02, 0x03, 0x04, 0x05,
                // CRC values. (Filled in the code)
                0xb7, 0x0e, 
                // End flag:
                0x7E
            };

        uint8_t TEST_VALUE[] = {
            // Correlation tag:
            0x06, 0xC7, 0xF4, 0xE8, 0x01, 0x02, 0x21, 0x93,
            // BEGIN ENCODED PART
            // Init flag:
            0x0f, 0x5c, 0xbc, 0x1d,
            // Destination address must match satellite's:
            0x81, 0x6e, 0x9c, 0x1a,
            0x39, 0xbd, 0x6c, 0x15,
            0x90, 0xca, 0xb0, 0xb9,
            0xd9, 0x13, 0xac, 0x46,
            0x70, 0x64, 0x7f, 0x43,
            0x4e, 0x6e, 0x93, 0xbc,
            0x12, 0xd2, 0x81, 0x61,
            // Source addresses
            0xcf, 0x0f, 0x53, 0x1a,
            0x39, 0xbd, 0x6c, 0x15,
            0x90, 0xca, 0xbf, 0xea,
            0x39, 0xbd, 0x6c, 0x15,
            0x90, 0xca, 0xbf, 0xea,
            0x39, 0xbd, 0x6c, 0x15,
            0x90, 0xca, 0x45, 0x7d,
            0x44, 0x82, 0x6c, 0x1a,
            0x39, 0xbd, 0x6c, 0x1a,
            0x39, 0xbd, 0x6c, 0x1a,
            0x39, 0xbd, 0x6c, 0x1a,
            0x39, 0xbd, 0x6c, 0x1a,
            0x39, 0xbd, 0x6c, 0x1a,
            0x39, 0xb2, 0x3f, 0x0f,
            // Control byte
            0xa9, 0x77, 0xd3, 0xf0,
            // PID byte
            0xf5, 0xcb, 0x34, 0xe6,
            // Payload should be here
            0x1c, 0xf0, 0x00, 0x0f,
            0xa9, 0x77, 0xd3, 0x0a,
            0x97, 0x7d, 0x3f, 0xf5,
            0x3e, 0x0a, 0xe3, 0x59,
            0x77, 0xd3, 0xff, 0xa6,
            // CRC
            0x24, 0xc6, 0x6f, 0x99,
            0xa5, 0x29, 0xe9, 0x34,
            // End flag
            0x92, 0xcd, 0x73, 0x1d,
            // END ENCODED PART
        };
#endif

/* Flag which indicates if it is the first byte to be appended to the masters buffer */
uint8_t first_byte_for_master;

#endif /* DEFINES_H_ */
