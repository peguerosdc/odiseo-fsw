/**
 * @file uart.h
 * @author Carlos Pegueros
 * @brief UART master driver
 */

#ifndef COMMS_COMMS_H
#define COMMS_COMMS_H

#include <stdint.h>
#include <msp430.h>
#include <fx_25/helpers.h>
#include <fx_25/crc.h>
#include <radio_utils/radio_utils.h>

#define PAYLOAD_MAX_SIZE                    50

uint8_t OWN_ADDRESS[7];

/****************************************
 * Exposed methods to main
 ****************************************/
#define MASTER_TRANSMISSION_READY       BIT5

void    comms_init(void);

uint8_t comms_is_data_from_ground_available(void);

uint8_t comms_get_byte(void );

uint8_t comms_is_ready_to_transmit(void);

void    comms_transmit_pending_data(void);

void    comms_set_frame_address(uint8_t destination_addr[7]);

void    comms_add_to_master_buffer(uint8_t byte, uint8_t initFlag);

void    comms_set_frame_index(uint8_t nr);

void    comms_confirm_frame_received();

void reply_spi();

/****************************************
 * I2C
 ****************************************/
#define I2C_SDA_PIN                   BIT7
#define I2C_SCL_PIN                   BIT6
#define I2C_MASTER_TRIGGER            BIT0
#define I2C_OWN_ADDRESS               0x48

#define COMMAND_ASK_FOR_FRAME           0xFE

#define EOF                 0xFF
/* The helper index to keep track of the data sent to the master uC */
volatile uint8_t master_transmit_counter;
/* The index of the next byte in storedBytes. This also represents the total amount of bytes received from the radio */
uint8_t storageIndex;

struct FrameData {
    uint8_t index;
    uint8_t address[7];
    uint8_t payload[PAYLOAD_MAX_SIZE];
};

struct FrameData to_master_buffer_container;
struct FrameData from_master_buffer_container;

/****************************************
 * UART
 ****************************************/
#define UART_RX_PIN              BIT1
#define UART_TX_PIN              BIT2

#define BUFFER_MAX_LEN      32

/****************************************
 * Rx Methods
 ****************************************/
volatile uint8_t rx_stack[BUFFER_MAX_LEN];
volatile uint8_t head;
volatile uint8_t tail;

/****************************************
 * Tx Methods
 ****************************************/
volatile uint8_t data_to_transmit;
/* The total amount of bytes received from the master uC to be transmitted */
volatile uint8_t tx_count;

#endif /* COMMS_COMMS_H */
