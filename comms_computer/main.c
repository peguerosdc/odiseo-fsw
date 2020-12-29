

#include <defines.h>

#define AX25_INIT_END_FLAG                                  0x7E

// Current state
void (*state_machine)(uint8_t);

int main(void) {
    // Perform setup
    initBoard();
    put_radio_receive_mode();

    // Main loop
    uint8_t initDecoder = 1;
    state_machine = NULL;
    // FEC control variables
    uint8_t fecBuffer[4];
    uint8_t fecCounter = 4;
    while(1)
    {
        // Process received bytes if there are any
        while( comms_is_data_from_ground_available() )
        {
            uint8_t received;
            received = comms_get_byte();
            // Always look for correlation tag
            if( FindCorrelationTag(received) == 1 ) {
                initDecoder = 1;
                state_machine = &state1;
                continue;
            }
            // If not, start decoding set of four bytes
            if(state_machine != NULL){
                fecBuffer[fecCounter-(uint8_t)1] = received;
                fecCounter--;
                if(fecCounter == 0) {
                    fecCounter = 4;
                    // replace received with the decoded byte
                    uint8_t decoded;
                    decoded = decodeByte(*(uint32_t*)fecBuffer, initDecoder);
                    // Execute the proper action
                    (*state_machine)(decoded);
                    initDecoder = 0;
                }
            }
        }

        // Send bytes if there are any
        if( comms_is_ready_to_transmit() ) {
            /* The transmission is busy at the moment */
            P1OUT &= ~MASTER_TRANSMISSION_READY;
            /* Transmit data */
            comms_transmit_pending_data();
            /* When the transmission is over, notify the master in case it wants to send more information */
            notify_master_transmission_ready();
        }

        __bis_SR_register(LPM3_bits + GIE);

    }
    return 0;
}

void initBoard()
{
    // Stop watchdog timer
    WDTCTL = WDTPW + WDTHOLD;
    // Setup internal clock to 1MHz
    DCOCTL = 0;
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
    // Configure P1 to output on P1.0
    P1DIR |= BIT0;
    /* Init communication modules */
    comms_init();
    // Notify the master it can transmit
    notify_master_transmission_ready();
    // Enable interruptions
    __enable_interrupt();
}

void state1(uint8_t rx_byte) {
    // We are waiting for the AX25 flag
    if( rx_byte == AX25_INIT_END_FLAG) {
        // Promote to the next state
        state_machine = &state2;
        // Init variables for state 2
        addressCounter = 0;
        destinationAddress = 0;
        source_address_index = 0;
        // Init variables for buffer
        storageIndex = 0;
    }
    else {
        state_machine = NULL;
    }
}

void state2(uint8_t rx_byte){
    // Start CRC
    compute_crc_of_byte(addressCounter, rx_byte);
    // Build the destination address with the first 7 bytes
    addressCounter++;
    if( addressCounter <= 7 ) {
        destinationAddress = (destinationAddress << 8) | rx_byte;
    }
    else {
        if( destinationAddress == *((uint64_t*)&OWN_ADDRESS) ) {
            // Receive next source frames and store the last one which indicates
            // the source address
            sourceAddress[source_address_index] = rx_byte;
            source_address_index++;
            if( addressCounter - 7 == 7) {
                if( rx_byte & 0x01 ) {
                    state_machine = &state3;
                    comms_set_frame_address(sourceAddress);
                }
                else {
                    // Reset source address and keep looking
                    source_address_index = 0;
                }
                addressCounter = 7;
            }
        }
        else {
            // Reset machine state and discard this frame
            state_machine = NULL;
        }
    }
}

void state3(uint8_t rx_byte) {
    // Receive control byte. Only modulo 8 control bytes are supported, which is the default mode
    // The other mode is 180 modulo and those control values are 2 bytes long
    if( !rx_byte & 0x01) {
        // Store index for future confirmation
        comms_set_frame_index( rx_byte>>1 & 0x07 );
        // Information frame received
        state_machine = &state4;
        compute_crc_of_byte(CRC_STATE_MACHINE_NORMAL_OP, rx_byte);
    }
    else {
        /* Supervisory frames are part of AX.25 and comply with (rx_byte & 0x01)
         * Unnumbered frames are part of AX.25 and comply with (rx_byte & 0x03)
         * But for the purpouses of this project, they are going to be discarded
         */
        // No supported frame was found. Discard frame
        state_machine = NULL;
    }
}

void state4(uint8_t rx_byte) {
    // No layer 3 protocol must be implemented
    if( rx_byte == 0xF0 ) {
        compute_crc_of_byte(CRC_STATE_MACHINE_NORMAL_OP, rx_byte);
        first_byte_for_master = 1;
        state_machine = &state5;
    }
    // Otherwise, the frame is discarded as it can not be processed
    else {
        state_machine = NULL;
    }
}

void state5(uint8_t rx_byte) {
    if( rx_byte == AX25_INIT_END_FLAG ) {
        // Check CRC
        if( is_crc_valid() ) {
            // Notify the master that there is data ready to be sent
            notify_master_data_available();
            // Confirm frame reception
            comms_confirm_frame_received();
            // Then, reset:
            state_machine = NULL;
        }
        // Discard frame
        else {
            state_machine = NULL;
        }
    }
    else {
        compute_crc_of_byte(CRC_STATE_MACHINE_NORMAL_OP, rx_byte);
        comms_add_to_master_buffer(rx_byte, first_byte_for_master);
        first_byte_for_master = 0;
    }
}

void notify_master_data_available() {
    // cambia un PIN para informarle al maestro que hay una trama recibida que mandarle
    P1OUT |= I2C_MASTER_TRIGGER;
    P1OUT &= ~I2C_MASTER_TRIGGER;
}


void notify_master_transmission_ready(){
    // cambia un PIN para informarle al maestro que el micro esta disponible para transmitir informacion
    //P1OUT &= ~MASTER_TRANSMISSION_READY;
    P1OUT |= MASTER_TRANSMISSION_READY;
}

