/**
 * @file ds3231.c
 * @author Carlos Pegueros
 * @brief Functions to interface with DS3231
 */
#include <ds3231/ds3231.h>

static uint8_t bcd2bin (uint8_t val){
    return val - 6 * (val >> 4);
}


static uint8_t bin2bcd (uint8_t val){
    return val + 6 * (val / 10);
}

uint8_t setDatetime(struct DS3231_DATETIME* settings) {
    // Build the transaction
    uint8_t tx_data[8] = {
        DS3231_REG_SECONDS,
        bin2bcd(settings->seconds),
        bin2bcd(settings->minutes),
        bin2bcd(settings->hours), // This defaults to 24 hour mode
        bin2bcd(settings->dow),
        bin2bcd(settings->day),
        bin2bcd(settings->month),
        bin2bcd(settings->year)
    };
    struct i2c_data data = i2c_build_bundle((uint8_t *) tx_data, 8, NULL, 0);
    // Perform transaction
    return i2c_transfer(DS3231_I2C_ADDRESS, &data);
}

uint8_t getDatetime(struct DS3231_DATETIME* container) {
    // Build the transaction
    uint8_t buffer[7] = {0,0,0,0,0,0,0};
    uint8_t tx_data[1] = {DS3231_REG_SECONDS};
    // Transform from BCD to normal numbers
    // {seconds, minutes, hours, day, date, month, year}
    const uint8_t datetimeMask[] = {0x7F,0x7F,0x3F,0x7,0x3F,0x1F,0xFF};
    struct i2c_data data = i2c_build_bundle((uint8_t *) tx_data, 1, (uint8_t *)buffer, 7);
    // Perform transaction
    uint8_t result = i2c_transfer(DS3231_I2C_ADDRESS, &data);
    if( result == 0) {
        // Parse
        container->seconds = bcd2bin(buffer[6] & datetimeMask[0]);
        container->minutes = bcd2bin(buffer[5] & datetimeMask[1]);
        container->hours = bcd2bin(buffer[4] & datetimeMask[2]);
        container->dow = bcd2bin(buffer[3] & datetimeMask[3]);
        container->day = bcd2bin(buffer[2] & datetimeMask[4]);
        container->month = bcd2bin(buffer[1] & datetimeMask[5]);
        container->year = bcd2bin(buffer[0] & datetimeMask[6]);
    }
    return result;
}
