/**
 * @file bmp180.c
 * @author Carlos Pegueros
 * @brief Functions to interface with BMP180
 */

#ifndef BMP180_BMP180_H_
#define BMP180_BMP180_H_

#include <i2c/i2c.h>
#include <msp430.h>

//#define BMP180_COMPLETE

#define BMP180_I2C_ADDRESS              (0x77)
#define BMP180_CHIPID                   (0x55)
#define BMP180_COMMAND_TEMPERATURE      (0x2E)
#define BMP180_COMMAND_PRESSURE         (0x34)

#define BMP180_REG_CHIP_ID              (0xD0)
#define BMP180_REG_CONTROL              (0xF4)
#define BMP180_REG_RESULT               (0xF6)

/* Highest operating frequency for BMP180 is 1 sample every 7.8ms */
#define BMP180_MAX_TASK_PERIOD          (8)

#define BMP180_MIN_TEMPERATURE_WAIT     (5)

#define BMP180_PRESSURE_0_TIME          (5)
#define BMP180_PRESSURE_1_TIME          (8)
#define BMP180_PRESSURE_2_TIME          (14)
#define BMP180_PRESSURE_3_TIME          (26)
#define BMP180_MIN_PRESSURE_WAIT(os)    (os == 0 ? BMP180_PRESSURE_0_TIME : (os == 1 ? BMP180_PRESSURE_1_TIME : ( os == 2 ? BMP180_PRESSURE_2_TIME : BMP180_PRESSURE_3_TIME)))

struct BMP180_calibration_data {
    int16_t    ac1;
    int16_t    ac2;
    int16_t    ac3;
    uint16_t   ac4;
    uint16_t   ac5;
    uint16_t   ac6;
    int16_t    b1;
    int16_t    b2;
    int16_t    mb;
    int16_t    mc;
    int16_t    md;
};

#ifdef BMP180_COMPLETE

    #define BMP180_REG_CALIBRATION_START (0xAA)

    int BMP180_readID(void);

    int BMP180_readCalibrationData(struct BMP180_calibration_data* container);

#endif

uint8_t BMP180_requestTemperature(void);

/*
 * Return in celsius
 */
uint16_t BMP180_readRawTemperature(void);

int16_t BMP180_computeTemperature(struct BMP180_calibration_data bmp180Calibrated, uint16_t rawTemperature);


uint8_t BMP180_requestPressure(uint8_t oversampling);

/*
 * Return in hPa.
 */
float BMP180_readPressure(struct BMP180_calibration_data bmp180Calibrated, uint8_t oversampling, uint16_t rawTemperature);

#endif /* BMP180_BMP180_H_ */
