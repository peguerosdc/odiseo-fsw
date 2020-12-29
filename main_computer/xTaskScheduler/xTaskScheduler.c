/*
 * xTaskScheduler.c
 *
 *  Created on: 04/10/2017
 *      Author: alponced
 */


#include <xTaskScheduler/xTaskScheduler.h>

uint8_t taskIndex = 0;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Core interrupt of the scheduler
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint8_t TimerFlag = 0;
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {
   TimerFlag = 1;
   __bic_SR_register_on_exit(LPM3_bits);
   return;
}

void setInterruptPeriod(uint32_t value) {
    interruptPeriod = value;
}

int8_t changeTaskPeriod(uint8_t task_id, uint32_t task_period) {
    uint8_t i = 0;
    while( i < taskIndex) {
        Task* t = &tasks[i];
        if( t->id == task_id ) {
            t->period = task_period;
            return i;
        }
        i++;
    }
    return -1;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Core of the scheduler
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void xCreateTask(uint8_t id, uint32_t period, TaskType task_type, void* function) {
    // Create task
    Task task;
    task.id = id;
    task.period = period;
    task.elapsedTime = 0;
    task.TickFct = function;
    task.isPeriodic = task_type;
    task.isExecuted = 0;
    // Insert into the queue
    tasks[taskIndex] = task;
    taskIndex++;
}

void xDeinitScheduler() {
    // Traverse through all the list freeing memory
    taskIndex = 0;
}

void xInitScheduler() {
    // Heart of the scheduler's code
    while(1) {
        /* If no task was executed, wait for the timer to trigger again
         * and enter low-power mode 3 with enabled interruptions */
        TimerFlag = 0;
        while (!TimerFlag) { _BIS_SR(LPM3_bits + GIE); }
        /* Traverse to every task looking for the one that goes next.
         * The first task in the list has the highest priority */
        uint8_t prev = 0;
        while(prev < taskIndex) {
            WDTCTL = WDT_ARST_1000;  // Put WDT+ in Watch Dog Mode
            Task* crawler = &tasks[prev];
            /* If the task is to be executed ONCE */
            if( !(crawler->isPeriodic == TASK_ONCE_IN_A_LIFETIME && crawler->isExecuted == 1) ) {
                /* Run each task if it is time to run it */
                crawler->elapsedTime += interruptPeriod;
                if ( crawler->elapsedTime >= crawler->period) {
                    // Run task
                    crawler->TickFct();
                    crawler->isExecuted = 1;
                    /* Check if the task must not be re-scheduled */
                    if( crawler->isPeriodic == TASK_PERIODIC ) {
                        // Reset counter of time
                        crawler->elapsedTime = 0;
                    }
                }
            }
            prev++;
        }
   }
}
