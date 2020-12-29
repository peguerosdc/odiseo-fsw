/*
 * radio_utils.h
 *
 *  Created on: 30/10/2017
 *      Author: peguerosdc
 */

#ifndef RADIO_UTILS_H_
#define RADIO_UTILS_H_

/*
 * @brief inits all the peripherials and control variables for the radio
 */
void initRadio();

/*
 * @brief puts the radio in transmit mode
 */
void put_radio_transmit_mode();

/*
 * @brief puts the radio in receive mode
 */
void put_radio_receive_mode();

#endif /* RADIO_UTILS_H_ */
