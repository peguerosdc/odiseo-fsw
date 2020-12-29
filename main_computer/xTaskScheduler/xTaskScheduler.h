/*
 * xTaskScheduler.h
 *
 *  Created on: 04/10/2017
 *      Author: alponced
 */

#ifndef XTASKSCHEDULER_H_
#define XTASKSCHEDULER_XTASKSCHEDULER_H_

#include <stdlib.h>
#include <stdint.h>
#include <msp430.h>

/**
 * @brief definitions of the available types of tasks
 */
typedef enum
{
    /** The task is set to be repeated forever */
    TASK_PERIODIC,
    /** The task is set to be executed only once */
    TASK_ONCE_IN_A_LIFETIME,
} TaskType;

/**
 * @brief Definition of a Task
 */
typedef struct Task
{
    uint8_t id;
    /** Rate at which the task should tick */
    uint32_t period;
    /** Time since task's last tick. Only for internal purposes */
    uint32_t elapsedTime;
    /** Function to call for task's tick */
    void (*TickFct)(void);
    /** 1 if the task shall be executed more than once **/
    TaskType isPeriodic;
    /** Is executed */
    uint8_t isExecuted;
} Task;

/**
 * Stores the amount of time between every Timer interruption
 */
uint32_t interruptPeriod;

/**
 * @brief Sets the scheduler to compute times according to the timer's settings
 *
 * @param[in] period the period of the timer interruption in milliseconds
 */
void setInterruptPeriod(uint32_t);

/**
 * Queue of tasks
 */
Task tasks[6];

/**
 * @brief Create a periodic task
 *
 * @param[in] period the period of the task in milliseconds
 * @param[in] task_type type of scheduling for this task
 * @param[in] function the address of the function to be executed
 */
void xCreateTask(uint8_t id, uint32_t period, TaskType task_type, void* function);

/**
 * @brief Perform the scheduler
 *
 * This is a blocking function which will never return as it will
 * be in an infinite loop scheduling the tasks. This also puts the
 * microcontroller to sleep (TODO) as everything is based on TimerA
 * interruptions.
 */
void xInitScheduler(void);

/**
 * @brief Free all the assigned memory by the scheduler
 */
void xDeinitScheduler(void);

/**
 * @brief recover one task
 */
int8_t changeTaskPeriod(uint8_t task_id, uint32_t task_period);

#endif /* XTASKSCHEDULER_XTASKSCHEDULER_H_ */
