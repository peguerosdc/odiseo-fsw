/**
 * @file i2c.h
 * @author Carlos Pegueros
 * @brief I2C master driver based on the work of Chris Karaplis<simplyembedded.org>
 */

#ifndef I2C_I2C_H_
#define I2C_I2C_H_

#include <stdint.h>
#include <stddef.h>
#include <msp430.h>

#define I2C_SDA_PIN                   BIT7
#define I2C_SCL_PIN                   BIT6
#define I2C_MASTER_TRIGGER            BIT0
#define TRANSMISSION_READY            BIT1

/**
 * @brief Definition of what a transaction contains in I2C.
 */
struct i2c_data {
    /** Data to send via I2C */
    const void *tx_data;
    /** Length of the data to send via I2C */
    size_t tx_len;
    /** Buffer to store received data via I2C */
    void *rx_buffer;
    /** Length of the buffer to store received data via I2C */
    size_t rx_len;
};

/**
 * @brief Initialize the USCI register to implement I2C
 *
 * Set UCB0CTL0 to master mode (UCMST) and for I2C (UCMODE_3 | UCSYNC).
 * Set the speed of the bus to be at SMCLK/10 bps (UCB0BR0 and UCB0BR1)
 *
 * @return 0 on success, -1 otherwise
 */
int i2c_init(void);

/**
 * @brief Build a bundle of what a transaction will send and receive
 *
 * @param[in] tx_buffer bytes to be sent to a slave
 * @param[in] rx_buffer memory to store the expected bytes to be received from the slave
 * @param[in] expectedBytes the number of bytes to be received
 *
 * @return a <tt>struct i2c_data</tt> containing the info of the transaction
 */
struct i2c_data i2c_build_bundle(const void* tx_buffer, size_t bytesToSend, void* rx_buffer, size_t expectedBytes);

/**
 * @brief Perform an I2C transaction
 *
 * @param[in] dev the I2C slave device
 * @param[in/out] data structure with the information about the transaction
 *
 * @return 0 on success, -1 otherwise
 */
int i2c_transfer(const uint8_t address, struct i2c_data *data);

#endif /* I2C_I2C_H_ */
