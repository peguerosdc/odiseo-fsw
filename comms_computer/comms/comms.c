/**
 * @file uart.h
 * @author Carlos Pegueros
 * @brief UART master driver
 */

#include <comms/comms.h>

/****************************************
 * Init Methods
 ****************************************/

static void _uart_init() {
    /* Configure ports for UART */
    P1SEL  |= UART_RX_PIN | UART_TX_PIN;
    P1SEL2 |= UART_RX_PIN | UART_TX_PIN;

    /* Ensure USCI_B0 is in reset before configuring */
    UCA0CTL1 = UCSWRST;

    /* Select SMCLK as the clock source */
    UCA0CTL1 |= UCSSEL_2;
    /* Set UCA0 to UART mode without baud rate detection pp 429, 433 */
    UCA0CTL0 = 0;
    UCA0ABCTL = 0;
    /* Setup baud rate to 9600 pp 424 */
    UCA0BR0 = 130;
    UCA0BR1 = 6;
    UCA0MCTL = 12;

    /* Take USCI_A0 out of reset */
    UCA0CTL1 &= ~UCSWRST;
    /* Enable USCI_A0 RX interrupt */
    UC0IE |= UCA0RXIE;

    /* Init Rx control flags */
    head = 0;
    tail = 0;
    /* Init Tx control flags */
    tx_count = 0;
    data_to_transmit = 0;
    /* Init master comm control flags */
    master_transmit_counter = 0;
}

static void _i2c_init() {
    // Setup pin for master trigger
    P1DIR |= I2C_MASTER_TRIGGER | MASTER_TRANSMISSION_READY;
    // Initially, transmission is always ready
    P1OUT = MASTER_TRANSMISSION_READY;
    // Initially, there is no frame available
    P1OUT &= ~I2C_MASTER_TRIGGER;

    P1SEL  |= I2C_SDA_PIN + I2C_SCL_PIN;               // Assign I2C pins to USCI_B0
    P1SEL2 |= I2C_SDA_PIN + I2C_SCL_PIN;               // Assign I2C pins to USCI_B0
    UCB0CTL1 |= UCSWRST;                      // Enable SW reset
    UCB0CTL0 = UCMODE_3 + UCSYNC;             // I2C Slave, synchronous mode
    UCB0I2COA = I2C_OWN_ADDRESS;                // set own (slave) address
    UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
    IE2 |= UCB0TXIE + UCB0RXIE;               // Enable TX interrupt
    UCB0I2CIE |= UCSTTIE;                     // Enable STT interrupt
}

/****************************************
 * Radio Rx Methods
 ****************************************/
uint8_t is_command_send_size = 0;
/*
 * This is shared for SPI, I2C and UART.
 */
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void) {
    /*
     * UART is in USCI A
     */
    if(IFG2 & UCA0RXIFG) {
        // Add to rx buffer if there is enough room
        if( (head - tail) != BUFFER_MAX_LEN ){
            uint8_t offset = head & (BUFFER_MAX_LEN-1);
            rx_stack[offset] = UCA0RXBUF;
            head++;
        }
    }
    /*
     * I2C is in USCI B
     */
    // check if this condition is fine
    if( UCB0STAT & UCSTTIFG ) {
        // Clear start condition init flag
        UCB0STAT &= ~UCSTTIFG;
        // Setup communication with the other uC variables
        master_transmit_counter = 0;
        tx_count = 0;
        //is_command_send_size = 0;
    }

    __bic_SR_register_on_exit(LPM3_bits);
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void) {
    // Transmit
    if (IFG2 & UCB0TXIFG) {
        // Reply to the command
        if( is_command_send_size == 1 ) {
            UCB0TXBUF = storageIndex + 6;
            is_command_send_size = 0;
            /* Pull down master pin */
            //P1OUT |= I2C_MASTER_TRIGGER;
        }
        // Send frame byte by byte to master
        else if( master_transmit_counter < storageIndex + 6 ) {
            uint8_t* data = (uint8_t*)&to_master_buffer_container;
            UCB0TXBUF = *(data + master_transmit_counter);
            master_transmit_counter++;
        }
        else {
            /* EOF flag as no more information is available */
            UCB0TXBUF = EOF;
        }
    }
    // Receive
    else {
        uint8_t received_byte = UCB0RXBUF;
        if( received_byte == COMMAND_ASK_FOR_FRAME) {
            is_command_send_size = 1;
        }
        // Store in the buffer
        // DONT else if, because this handles when the COMMAND_ASK_FOR_FRAME is in the data
        if( received_byte != EOF) {
            /* Add to the buffer */
            uint8_t* data = (uint8_t*)&from_master_buffer_container;
            *(data + tx_count) = received_byte;
            tx_count++;
        }
        else {
            /* Transmit to UART */
            data_to_transmit = 1;
        }
    }

    __bic_SR_register_on_exit(LPM3_bits);
}

/****************************************
 * Tx Methods
 ****************************************/

static void _transmit_byte(uint8_t to_transmit){
    while(!(IFG2 & UCA0TXIFG));
    UCA0TXBUF = to_transmit;
}

static void _send_encoded_four_bytes(uint32_t encoded){
    uint8_t i;
    uint8_t to_transmit = 0;

    for (i=4; i>0; i--){
        to_transmit = (uint8_t)((encoded >> 8*(i - 1)) & 255);
        _transmit_byte(to_transmit);
    }
}

/****************************************
 * Exposed methods
 ****************************************/
void comms_init() {
    OWN_ADDRESS[0] = 0xE0;
    OWN_ADDRESS[1] = 0x1F;
    OWN_ADDRESS[2] = 0x1E;
    OWN_ADDRESS[3] = 0x1D;
    OWN_ADDRESS[4] = 0x1C;
    OWN_ADDRESS[5] = 0x1B;
    OWN_ADDRESS[6] = 0x1A;
    _uart_init();
    _i2c_init();
}

uint8_t comms_is_data_from_ground_available(void) {
    return ((head - tail) != 0);
}

uint8_t comms_get_byte() {
    uint8_t offset = tail & (BUFFER_MAX_LEN-1);
    uint8_t temp = rx_stack[offset];
    tail++;
    return temp;
}

uint8_t comms_is_ready_to_transmit() {
    return data_to_transmit == 1;
}

void comms_set_frame_index(uint8_t nr) {
    to_master_buffer_container.index = nr;
}

void comms_set_frame_address(uint8_t destination_addr[7]) {
    to_master_buffer_container.address[0] = destination_addr[0];
    to_master_buffer_container.address[1] = destination_addr[1];
    to_master_buffer_container.address[2] = destination_addr[2];
    to_master_buffer_container.address[3] = destination_addr[3];
    to_master_buffer_container.address[4] = destination_addr[4];
    to_master_buffer_container.address[5] = destination_addr[5];
    to_master_buffer_container.address[6] = destination_addr[6];
}

void comms_add_to_master_buffer(uint8_t byte, uint8_t initFlag) {
    if( initFlag == 1) {
        storageIndex = 0;
    }
    to_master_buffer_container.payload[storageIndex] = byte;
    storageIndex++;
}

void comms_confirm_frame_received() {
    put_radio_transmit_mode();
    // Send supervisory frame
    uint32_t encoded = 0;
    uint8_t crc_count = 0;
    uint8_t i = 0;
    // FX.25 flag
    _transmit_byte(0x7E);
    _transmit_byte(0x7E);
    _transmit_byte(0x7E);
    _transmit_byte(0x7E);
    // Correlation flag
    _transmit_byte(0x06);
    _transmit_byte(0xC7);
    _transmit_byte(0xF4);
    _transmit_byte(0xE8);
    _transmit_byte(0x01);
    _transmit_byte(0x02);
    _transmit_byte(0x21);
    _transmit_byte(0x93);
    // AX.25 init flag
    encoded = encodeByte((uint8_t)0x7E, (uint8_t)1);
    _send_encoded_four_bytes(encoded);
    // Destination address
    for(i = 0; i<7; i++) {
        encoded = encodeByte(*(to_master_buffer_container.address+i), (uint8_t)0);
        _send_encoded_four_bytes(encoded);
        compute_crc_of_byte(crc_count, *(to_master_buffer_container.address+i));
        crc_count++;
    }
    // Source address
    for(i = 0; i<7; i++) {
        encoded = encodeByte(OWN_ADDRESS[i], (uint8_t)0);
        _send_encoded_four_bytes(encoded);
        compute_crc_of_byte(crc_count, OWN_ADDRESS[i]);
    }
    // Control (Receive / Ready)
    uint8_t control = 0x01 | (to_master_buffer_container.index << 5);
    encoded = encodeByte(control, (uint8_t)0);
    _send_encoded_four_bytes(encoded);
    compute_crc_of_byte(crc_count, control);
    // CRC
    uint16_t crc = get_crc();
    uint8_t* crc_array = (uint8_t*)&crc;
    for (i=0; i<2; i++){
        encoded = encodeByte( *(crc_array+i), (uint8_t)0);
        _send_encoded_four_bytes(encoded);
    }
    // AX.25 end flag
    encoded = encodeByte((uint8_t)0x7E, (uint8_t)1);
    _send_encoded_four_bytes(encoded);
    // 2 bytes of end FX.25 flag
    for (i=0; i<2; i++){
        _transmit_byte(0x7E);
    }
    // END
    put_radio_receive_mode();
}

void comms_transmit_pending_data() {
    /* Put the radio in Tx mode */
    put_radio_transmit_mode();

    // Send bytes to the radio
    // 4(fxflag) + 8(corrflag) + 1(axflag) + 7(dstadd) + 7(srcadd) + 1(control) + 1(pid) + payload + 2(crc) + 1(axflag) + 2(fxflag)
    // 34 bytes + payload
    uint8_t* bytes_to_send = (uint8_t*)&from_master_buffer_container.payload;
    uint8_t index_payload = 0;
    uint8_t to_transmit = 0;
    uint32_t encoded = 0;
    uint8_t to_encode = 0;
    uint8_t i;

    /***** Hardcoded pieces of frame  ********/
    uint8_t correlation_flag[8] = {0x06, 0xC7, 0xF4, 0xE8, 0x01, 0x02, 0x21, 0x93};
    const uint8_t control =  to_master_buffer_container.index<<5 | from_master_buffer_container.index<<1;
    const uint8_t pid = 0xF0;
    uint8_t crc_count = 0;
    /*****************************************/

    // 4 bytes of init FX.25 flag
    to_transmit = 0x7E;
    for (i=0; i<4; i++){
        _transmit_byte(to_transmit);
    }
    // 8 bytes of correlation flag
    for (i=0; i<8; i++){
        to_transmit = correlation_flag[i];
        _transmit_byte(to_transmit);
    }
    // AX.25 init flag
    encoded = encodeByte((uint8_t)0x7E, (uint8_t)1);
    _send_encoded_four_bytes(encoded);
    // 7 bytes for dest address
    for (i=0; i<7; i++){
        encoded = encodeByte(from_master_buffer_container.address[i], (uint8_t)0);
        _send_encoded_four_bytes(encoded);
        compute_crc_of_byte(crc_count, from_master_buffer_container.address[i]);
        crc_count++;
    }
    // 7 bytes for src address
    for (i=0; i<7; i++){
        encoded = encodeByte(OWN_ADDRESS[i], (uint8_t)0);
        _send_encoded_four_bytes(encoded);
        compute_crc_of_byte(crc_count, OWN_ADDRESS[i]);
        crc_count++;
    }
    // 1 byte for control
    encoded = encodeByte(control, (uint8_t)0);
    _send_encoded_four_bytes(encoded);
    compute_crc_of_byte(crc_count, control);
                crc_count++;
    // 1 byte for pid
    encoded = encodeByte(pid, (uint8_t)0);
    _send_encoded_four_bytes(encoded);
    compute_crc_of_byte(crc_count, pid);
                crc_count++;
    // bytes for payload
    while(index_payload < tx_count - 8){
        to_encode = *(bytes_to_send + index_payload);
        encoded = encodeByte(to_encode, (uint8_t)0);
        _send_encoded_four_bytes(encoded);
        index_payload++;
        // crc
        compute_crc_of_byte(crc_count, to_encode);
                    crc_count++;
    }
    // 2 bytes for crc
    uint16_t crc = get_crc();
    uint8_t* crc_array = (uint8_t*)&crc;
    for (i=0; i<2; i++){
        encoded = encodeByte( *(crc_array+i), (uint8_t)0);
        _send_encoded_four_bytes(encoded);
    }
    // AX.25 end flag
    encoded = encodeByte((uint8_t)0x7E, (uint8_t)0);
    _send_encoded_four_bytes(encoded);
    // 2 bytes of end FX.25 flag
    to_transmit = 0x7E;
    for (i=0; i<2; i++){
        _transmit_byte(to_transmit);
    }

    /* Reset the radio to Rx mode */
    put_radio_receive_mode();
    /* Reset control variables */
    data_to_transmit = 0;
    tx_count = 0;
}
