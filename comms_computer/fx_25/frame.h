/*
 * frame.h
 *
 *  Created on: 10/10/2017
 *      Author: alponced
 */

#ifndef FX_25_FRAME_H_
#define FX_25_FRAME_H_

/**
 * @brief FX.25 frame definition
 * AX
 **/
typedef struct Fx25_frame {
    /** Flag to initiate */
    uint8_t initfx_flag;
    /** Correlation flag for synchrony */
    uint64_t correlation_flag;
    /** BEGIN of AX.25 frame */
    /** Flag to initiate */
    uint8_t initax_flag;
    /** Source address of the frame */
    uint64_t src_address;
    /** Destination address of the frame */
    uint64_t dst_address;
    /** Type of frame */
    uint8_t control;
    /** Type of protocol */
    uint8_t protocol;
    /** Payload of the frame **/
    uint64_t payload;
    /** To check for errors **/
    uint16_t fcs;
    /** Flag to end */
    uint8_t endax_flag;
    /** END of AX.25 frame */
    /* Padding to FEC */
    uint8_t padding;
    /** Flag to end */
    uint8_t endfx_flag;
} Fx25_frame;


#endif /* FX_25_FRAME_H_ */
