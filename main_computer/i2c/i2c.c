/**
 * @file i2c.c
 * @author Carlos Pegueros
 * @brief I2C master driver based on the work of Chris Karaplis<simplyembedded.org>
 */

#include <i2c/i2c.h>

static int _transmit(const uint8_t *buf, size_t nbytes);
static int _receive(uint8_t *buf, size_t nbytes);
static int _check_ack();

struct i2c_data i2c_build_bundle(const void* tx_data, size_t bytesToSend, void* rx_buffer, size_t expectedBytes) {
    struct i2c_data data;
    data.tx_data = tx_data;
    data.tx_len = bytesToSend;
    data.rx_buffer = rx_buffer;
    data.rx_len = expectedBytes;
    return data;
}


int i2c_init(void) {
    // Init I2C on USCI_B0
    P1SEL |= I2C_SDA_PIN + I2C_SCL_PIN;
    P1SEL2|= I2C_SDA_PIN + I2C_SCL_PIN;

    /* Ensure USCI_B0 is in reset before configuring */
    UCB0CTL1 = UCSWRST;

    /* Set USCI_B0 to master mode I2C mode */
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;

    /**
     * Configure the baud rate registers for 100kHz when sourcing from SMCLK
     * where SMCLK = 1MHz
     */
    UCB0BR0 = 10;
    UCB0BR1 = 0;

    /* Take USCI_B0 out of reset and source clock from SMCLK */
    UCB0CTL1 = UCSSEL_2;

    /* Setup interrupt pins to communicate with slave uC */

    // P2.0 is going to be used to notify this uC that there is a frame ready to be processed
    P2SEL &= (~I2C_MASTER_TRIGGER); // Set P2.0 SEL as GPIO
    P2DIR &= (~I2C_MASTER_TRIGGER); // Set P2.0 SEL as Input
    P2IES &= (~BIT0); // Falling Edge
    P2IFG &= (~I2C_MASTER_TRIGGER); // Clear interrupt flag for P2.0
    P2IE |= (I2C_MASTER_TRIGGER); // Enable interrupt for P2.0

    // P2.1 is going to be used to nofity this uC that transmission is available
    P2SEL &= (~TRANSMISSION_READY); // Set P2.1 SEL as GPIO
    P2DIR &= (~TRANSMISSION_READY); // Set P2.1 SEL as Input

    return 0;
}


int i2c_transfer(const uint8_t address, struct i2c_data *data) {
    int err = 0;

    /* Set the slave device address */
    UCB0I2CSA = address;

    /* Transmit data is there is any */
    if (data->tx_len > 0) {
        err = _transmit((const uint8_t *) data->tx_data, data->tx_len);
    }

    /* Receive data is there is any */
    if ((err == 0) && (data->rx_len > 0)) {
        err = _receive((uint8_t *) data->rx_buffer, data->rx_len);
    } else {
        /* No bytes to receive send the stop condition */
        UCB0CTL1 |= UCTXSTP;
    }

    return err;
}


static int _check_ack() {
    int err = 0;

    /* Check for ACK */
    if (UCB0STAT & UCNACKIFG) {
        /* Stop the I2C transmission */
        UCB0CTL1 |= UCTXSTP;

        /* Clear the interrupt flag */
        UCB0STAT &= ~UCNACKIFG;

        /* Set the error code */
        err = -1;
    }

    return err;
}

static int _transmit(const uint8_t *buf, size_t nbytes) {
    int err = 0;

    /* Send the start condition */
    UCB0CTL1 |= UCTR | UCTXSTT;

    /* Wait for the start condition to be sent and ready to transmit interrupt */
    while ((UCB0CTL1 & UCTXSTT) && ((IFG2 & UCB0TXIFG) == 0));

    /* Check for ACK */
    err = _check_ack();

    /* If no error and bytes left to send, transmit the data */
    while ((err == 0) && (nbytes > 0)) {
        UCB0TXBUF = *buf;
        while ((IFG2 & UCB0TXIFG) == 0) {
            err = _check_ack();
            if (err < 0) {
                break;
            }
        }

        buf++;
        nbytes--;
    }

    return err;
}

static int _receive(uint8_t *buf, size_t nbytes) {
    int err = 0;

    /* Send the start and wait */
    UCB0CTL1 &= ~UCTR;
    UCB0CTL1 |= UCTXSTT;

    /* Wait for the start condition to be sent */
    while (UCB0CTL1 & UCTXSTT);

    /*
     * If there is only one byte to receive, then set the stop
     * bit as soon as start condition has been sent
     */
    if (nbytes == 1) {
        UCB0CTL1 |= UCTXSTP;
    }

    /* Check for ACK */
    err = _check_ack();

    /* If no error and bytes left to receive, receive the data */
    /*< Invert the bytes */
    //buf = buf + nbytes - 1;
    while ((err == 0) && (nbytes > 0)) {
        /* Wait for the data */
        while ((IFG2 & UCB0RXIFG) == 0);

        *buf = UCB0RXBUF;
        buf++;
        nbytes--;

        /*
         * If there is only one byte left to receive
         * send the stop condition
         */
        if (nbytes == 1) {
            UCB0CTL1 |= UCTXSTP;
        }
    }

    return err;
}
