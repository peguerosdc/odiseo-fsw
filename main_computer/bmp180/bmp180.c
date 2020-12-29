/**
 * @file bmp180.c
 * @author Carlos Pegueros
 * @brief Functions to interface with BMP180
 */


#include <bmp180/bmp180.h>

#ifdef BMP180_COMPLETE

    int BMP180_readID() {
        int result = -1;
        // Build the transaction
        uint8_t tx_data[1] = {BMP180_REG_CHIP_ID};
        uint8_t rx_data[1];
        struct i2c_data data = i2c_build_bundle((uint8_t *) tx_data, 1, (uint8_t *) rx_data, 1);
        // Perform transaction
        int err = i2c_transfer(BMP180_I2C_ADDRESS, &data);
        if( err == 0 ) {
            if( rx_data[0] == (uint8_t)BMP180_CHIPID ) {
                result = 0;
            }
        }
        return result;
    }

    int BMP180_readCalibrationData(struct BMP180_calibration_data* container) {
        // TX Data will contain the register to read
        uint8_t tx_data;
        // Read the 11 calibration registers
        short cont = 0;
        while( cont < 11 ) {
            // Read each register
            tx_data = BMP180_REG_CALIBRATION_START + 2*cont;
            // Build transactions
            int16_t rx_data;
            struct i2c_data data = i2c_build_bundle((uint8_t *)&tx_data, 1, (uint8_t *)&rx_data, 2);
            // Perform transaction
            if( i2c_transfer(BMP180_I2C_ADDRESS, &data) == 0 ) {
                switch( cont ) {
                    case 0:
                        container->ac1 = rx_data;
                        break;
                    case 1:
                        container->ac2 = rx_data;
                        break;
                    case 2:
                        container->ac3 = rx_data;
                        break;
                    case 3:
                        container->ac4 = rx_data;
                        break;
                    case 4:
                        container->ac5 = rx_data;
                        break;
                    case 5:
                        container->ac6 = rx_data;
                        break;
                    case 6:
                        container->b1 = rx_data;
                        break;
                    case 7:
                        container->b2 = rx_data;
                        break;
                    case 8:
                        container->mb = rx_data;
                        break;
                    case 9:
                        container->mc = rx_data;
                        break;
                    case 10:
                        container->md = rx_data;
                        break;
                    default:
                        return -1;
                }
            }
            else {
                return -1;
            }
            // Update counter
            cont++;
        }
        return 0;
    }

#endif

uint8_t BMP180_requestTemperature() {
    /* Ask for start of measurement */
    uint8_t tx_data[2] = {BMP180_REG_CONTROL, BMP180_COMMAND_TEMPERATURE};
    struct i2c_data data = i2c_build_bundle((uint8_t *)&tx_data, 2, NULL, 0);
    return i2c_transfer(BMP180_I2C_ADDRESS, &data);
}

uint16_t BMP180_readRawTemperature() {
    // Ask for lecture
    uint8_t tx_lecture_data = BMP180_REG_RESULT;
    uint16_t rawTemperature;
    struct i2c_data lecture = i2c_build_bundle((uint8_t *)&tx_lecture_data, 1, (uint8_t *)&rawTemperature, 2);
    if( i2c_transfer(BMP180_I2C_ADDRESS, &lecture) == 0 ) {
        rawTemperature = ( rawTemperature>>8 & 0xFF) | (rawTemperature<<8 & 0xFF00 );
        return rawTemperature;
    }
    return 1;
}

int16_t BMP180_computeTemperature(struct BMP180_calibration_data bmp180Calibrated, uint16_t rawTemperature) {
    int32_t calcX1 = (((int32_t)rawTemperature - (int32_t)bmp180Calibrated.ac6) * (int32_t)bmp180Calibrated.ac5) >> 15;
    int32_t calcX2 = ((int32_t)bmp180Calibrated.mc << 11) / (calcX1 + bmp180Calibrated.md);
    int16_t temperature = (calcX1 + calcX2 + 8) >> 4;
    return temperature;
}




uint8_t BMP180_requestPressure(uint8_t oversampling) {
    /* Ask for start of measurement */
    uint8_t tx_data[2] = {BMP180_REG_CONTROL, (BMP180_COMMAND_PRESSURE | (oversampling << 6) ) };
    struct i2c_data data = i2c_build_bundle((uint8_t *)&tx_data, 2, NULL, 0);
    return i2c_transfer(BMP180_I2C_ADDRESS, &data);
}

float BMP180_readPressure(struct BMP180_calibration_data bmp180Calibrated, uint8_t oversampling, uint16_t rawTemperature) {
    //rawTemperature = 27208;
    int32_t pressure;
    // Ask for lecture
    uint8_t tx_lecture_data = BMP180_REG_RESULT;
    uint32_t rawPressure;
    struct i2c_data lecture = i2c_build_bundle((uint8_t *)&tx_lecture_data, 1, (uint8_t *)&rawPressure, 4);
    if( i2c_transfer(BMP180_I2C_ADDRESS, &lecture) == 0 ) {
        //rawPressure = 35273;
        int32_t calcX1 = (((int32_t)rawTemperature - (int32_t)bmp180Calibrated.ac6) * (int32_t)bmp180Calibrated.ac5) >> 15;
        int32_t calcX2 = ((int32_t)bmp180Calibrated.mc << 11) / (calcX1 + bmp180Calibrated.md);
        int32_t calcB5 = calcX1 + calcX2;
        int32_t calcB6 = calcB5 - 4000;
        calcX1 = ((int32_t)bmp180Calibrated.b2 * (calcB6 * calcB6 >> 12)) >> 11;
        calcX2 = (int32_t)bmp180Calibrated.ac2 * calcB6 >> 11;
        int32_t calcX3 = calcX1 + calcX2;
        int32_t calcB3 = ((((int32_t)bmp180Calibrated.ac1 * 4 + calcX3) << oversampling) + 2) >> 2;
        calcX1 = (int32_t)bmp180Calibrated.ac3 * calcB6 >> 13;
        calcX2 = ((int32_t)bmp180Calibrated.b1 * (calcB6 * calcB6 >> 12)) >> 16;
        calcX3 = ((calcX1 + calcX2) + 2) >> 2;
        uint32_t calcB4, calcB7;
        calcB4 = (int32_t)bmp180Calibrated.ac4 * (uint32_t)(calcX3 + 32768) >> 15;
        calcB7 = ((uint32_t)rawPressure - calcB3) * (50000 >> oversampling);
        if(calcB7 < 0x80000000) pressure = ((calcB7 * 2) / calcB4);
        else pressure = (calcB7 / calcB4) * 2;
        calcX1 = (pressure >> 8) * (pressure >> 8);
        calcX1 = (calcX1 * 3038) >> 16;
        calcX2 = (-7357 * pressure) >> 16;
        pressure = pressure + ((calcX1 + calcX2 + 3791) >> 4);
        return (float)(pressure / 100.0);
    }
    return -1;
}
