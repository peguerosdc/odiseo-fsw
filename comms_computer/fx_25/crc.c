
#include <fx_25/crc.h>

/**
 * CRC Polynomial recommended by:
 * https://users.ece.cmu.edu/~koopman/crc/
 * Up to HD = 6 and 135 bits, length: 16 bits
 */
const uint16_t CRC_POLY = 0x9eb2;

uint16_t _residueCRC(uint8_t init, uint8_t byte_to_check) {
    static uint16_t crc; /* CRC value is 16bit */
    uint8_t i = 0;

    if (init == CRC_STATE_MACHINE_RESET_CRC){
        crc = 0;
    }

    crc ^= (uint16_t)(byte_to_check << 8); /* move byte into MSB of 16bit CRC */
    // 8 iterations to get from 16 to 8
    for (i = 8; i>0; i--) {
        // If bit 15 is one do CRC otherwise move one space left
        if ((crc & 0x8000) != 0) {
            crc = (uint16_t)((crc << 1) ^ CRC_POLY);
        }
        else {
            crc <<= 1;
        }
    }

    return crc;
}


/* Function to compute and store the crc byet by byte */
void _pushToCRC(uint8_t initFlag, uint8_t rx_byte) {
    uint8_t temp = (uint8_t)((candidateCRC & 0xFF00) >> 8);
    crc = _residueCRC(initFlag, temp);
    candidateCRC = (uint16_t)(candidateCRC<<8 | rx_byte);
}

void compute_crc_of_byte(uint8_t state_machine, uint8_t rx_byte) {
    if( state_machine == CRC_STATE_MACHINE_START) {
        candidateCRC = rx_byte;
    }
    else if( state_machine == CRC_STATE_MACHINE_POST_START) {
        candidateCRC = (uint16_t)(candidateCRC<<8 | rx_byte);
    }
    else {
        _pushToCRC(state_machine, rx_byte);
    }
}

uint8_t is_crc_valid() {
    return candidateCRC == crc;
}

uint16_t get_crc(){
    return crc;
}
