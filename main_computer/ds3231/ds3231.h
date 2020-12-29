/**
 * @file bmp180.c
 * @author Carlos Pegueros
 * @brief Functions to interface with BMP180
 */

#ifndef DS3231_DS3231_H_
#define DS3231_DS3231_H_

#include <stdint.h>
#include <i2c/i2c.h>

#define DS3231_I2C_ADDRESS                  (0x68)

#define DS3231_REG_SECONDS                  (0x00)
#define DS3231_REG_MINUTES                  (0x01)
#define DS3231_REG_HOURS                    (0x02)
#define DS3231_REG_DAY_OF_WEEK              (0x03)
#define DS3231_REG_DAY                      (0x04)
#define DS3231_REG_MONTH                    (0x05)
#define DS3231_REG_YEAR                     (0x06)

struct DS3231_DATETIME {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t dow;
	uint8_t day;
	uint8_t month;
	uint8_t year;
};



uint8_t setDatetime(struct DS3231_DATETIME*);

uint8_t getDatetime(struct DS3231_DATETIME*);

uint8_t getSecond(struct DS3231_DATETIME*);



#endif /* DS3231_DS3231_H_ */
