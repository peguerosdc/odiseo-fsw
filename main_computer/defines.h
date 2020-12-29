/*
 * defines.h
 *
 *  Created on: 04/10/2017
 *      Author: alponced
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <msp430.h>
#include <i2c/i2c.h>
#include <xTaskScheduler/xTaskScheduler.h>
#include <ds3231/ds3231.h>
#include <bmp180/bmp180.h>

#define PAYLOAD_MAX_SIZE                    50
#define COMMS_HANDLER_ADDRESS               0x48

/********************************************
 * Constants for initial low-power state
 ********************************************/

/** Maximum value for the TimerA counter */
#define TACCR0_MAX                          32767

/** Amount of time to wait before start of operation */
#define SLEEP_TIME_FOR_INIT                 50000 // 50 seconds just for testing
// #define SLEEP_TIME_FOR_INIT          10800000 // 3 hours

/*
 * Amount of milliseconds between every TIMER interruption
 * when TIMER is in hibernation state according to
 * /utils/compute_interrupt_period.py
 */
#define HIBERNATION_TIMER_PERIOD            21845

/*
 * Amount of milliseconds between every TIMER interruption
 * when TIMER is in normal state according to
 * /utils/compute_interrupt_period.py
 */
#define NORMAL_TIMER_PERIOD                 1


#define MAX_LOGS                            5

#define MEMORY_SIZE                         10

typedef struct log {
    struct DS3231_DATETIME datetime;
    char* message;
    uint8_t message_length;
} log;

log logs[MAX_LOGS];
uint8_t log_counter = 0;
static const log EmptyLog;

uint8_t memory[MEMORY_SIZE] = {255};

volatile uint8_t frame_ready;

/**
 * @brief Setup initial configuration for the peripherials board
 */
void initBoard(void);


/**********************************************
 * Main Tasks
 **********************************************/

/**
 * @brief task to occur after SLEEP_TIME_FOR_INIT [ms] after deployed
 *
 * This task is going to be executed when there is presumably enough power
 * to start operations after deployed. This is when the battery is charged
 * with enough current to power the main CPU and the radio for
 * communications with Earth
 */
#define TURN_ON_TASK_ID                         0
void TurnOnComputer(void);

/**
 * @brief task to process commands from Earth
 *
 * This task checks if there are pending commands to process from Earth
 * and it is going to act accordingly
 */
#define PROCESS_COMMAND_TASK_ID                 1
void ProcessCommand();

/**
 * @brief task to manage the power supply
 *
 * This task is meant to check if the batteries have enough energy to
 * power the main operations of the satellite. If the batteries do not
 * have enough energy, the satellite is going to use the backup battery
 * and put the former one to charge.
 */
#define POWER_MGMT_TASK_ID                      2
void CheckPowerManagement(void);

/**
 * @brief task to send a beacon to Earth
 *
 * This task is meant to send telemetry to Earth to notify that the satellite
 * is still alive. Telemtry to be sent is going to be temperature and pressure.
 */
#define BEACON_TASK_ID                  25
#define BEACON_BASE_PERIOD              30000
void SendTelemetry(void);
struct FrameData {
    uint8_t index;
    uint8_t address[7];
    uint8_t payload[PAYLOAD_MAX_SIZE];
};
uint8_t amount_of_data_to_send;
struct FrameData packet_to_send;
struct FrameData packet_received;
uint8_t data_pending_to_send;

#define SEND_PENDING_TASK_ID                    3
void SendPendingData(void);

/**
 * @brief task to store telemetry in the log
 *
 * This task is meant to store information in a log to keep track of what
 * has happened in the satellite. Information to be stored is:
 * - date and time
 * - status of the batteries
 * - telemetry
 */
#define STORE_LOG_TASK_ID                       4

void read_from_i2c(void);

void createLog(char* message);

void storeInLog(char* message);

void check_logs(log* checked_logs);

void clean_logs(void);

void get_logs_of_period(void);

void overwrite_memory(void);

void transmit_memory(void);

void clean_memory(void);


uint8_t more_data_to_send = 0;
uint8_t pending_command = 0;
uint8_t rx_amount = 0;

/*** send logs of period variables ****/
log send_logs[3];
uint8_t log_confirmation;
uint8_t log_i = 0;
uint8_t log_index = 0;
uint8_t log_payload_count = 0;
uint8_t log_struct_pos = 0;
uint8_t log_logs_to_check = 0;
/**************************************/

/*** transmit memory variables ****/
uint8_t transmit_memory_i = 0;
uint8_t transmit_payload_count = 0;
uint8_t transmit_memory_index = 0;
/**************************************/

/*** overwrite memory variables ****/
uint8_t overwrite_memory_i = 0;
uint8_t overwrite_payload_count = 1;
/**************************************/

/*** clean memory variables ****/
uint8_t clean_mem_i = 0;
/**************************************/

/*** clean log variables ****/
uint8_t clean_logs_i = 0;
/**************************************/

/*** store in log variables ****/
log new_log;
uint8_t log_msg_length;
/**************************************/

/*** check log variables ****/
uint8_t check_logs_i = 0;
uint8_t logs_created = 0;
/**************************************/

#endif /* DEFINES_H_ */
