#include <defines.h>

struct DS3231_DATETIME datetime;
struct BMP180_calibration_data bmp180Calibrated =
(struct BMP180_calibration_data) {
    .ac1 = 8324,
    .ac2 = -1127,
    .ac3 = -14476,
    .ac4 = 34357,
    .ac5 = 25169,
    .ac6 = 18855,
    .b1 = 6515,
    .b2 = 42,
    .mb = -32768,
    .mc = -11786,
    .md = 2717
};

/**
 * main.c
 */
int main(void) {
    // Perform setup
    initBoard();

    // If a software reset occurred:
    if( WDTIFG & IFG1 ) {
        IFG1 &= WDTIFG;
        TurnOnComputer();
    }
    else {
        // Create once in a lifetime wake-up task after three hours after deployed
        xCreateTask(TURN_ON_TASK_ID, 100, TASK_ONCE_IN_A_LIFETIME, &TurnOnComputer);
    }

    // Init scheduler
    xInitScheduler();

    // Free
    xDeinitScheduler();

    return 0;
}

void initBoard() {
    // Stop watchdog timer
    WDTCTL = WDTPW + WDTHOLD;
    // Setup internal master clock to 1 MHz
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    // Setup Auxiliary Clock ACLK to VLOCLK = 12kHz
    // BCSCTL3[LFXT1Sx] = b10
    BCSCTL3 |= BIT5;
    BCSCTL3 &= ~BIT4;
    // Configure everything as inputs
    P1DIR = 0;
    P2DIR = 0;

    //Configure P1 to output on P1.0
    P1DIR |= 0x01;
    P1OUT &= ~BIT0;

    // CCR0 interrupt enabled
    TACCTL0 = CCIE;

    // Set the TimerA for interruptions every HIBERNATION_TIMER_PERIOD ms
    setInterruptPeriod(HIBERNATION_TIMER_PERIOD);
    TACTL = TASSEL_1 + MC_1 + ID_3;
    TACCR0 =  TACCR0_MAX;

    // Enable interruptions
    __enable_interrupt();
}


void TurnOnComputer() {
    frame_ready = 0;
    data_pending_to_send = 0;
    // Speed up TimerA to interrupt every NORMAL_TIMER_PERIOD ms
    setInterruptPeriod(NORMAL_TIMER_PERIOD);
    TACTL = TASSEL_1 + MC_1 + ID_2;
    TACCR0 =  3;
    i2c_init();

    WDTCTL = WDT_ARST_1000;  // Put WDT+ in Watch Dog Mode
    /*
     * Init main computer tasks
     */
    xCreateTask(PROCESS_COMMAND_TASK_ID , 200, TASK_PERIODIC, &ProcessCommand);
    xCreateTask(SEND_PENDING_TASK_ID    , 100, TASK_PERIODIC, &SendPendingData);
    xCreateTask(POWER_MGMT_TASK_ID      , 60000, TASK_PERIODIC, &CheckPowerManagement);
    xCreateTask(BEACON_TASK_ID          , BEACON_BASE_PERIOD, TASK_PERIODIC, &SendTelemetry);
    xCreateTask(STORE_LOG_TASK_ID       , 1800000, TASK_PERIODIC, &storeInLog);
}

// Port 2 interrupt service routine for I2C comm (Pin P2.0)
#pragma vector = PORT2_VECTOR
__interrupt void Port_2(void)
{
    if( P2IFG & I2C_MASTER_TRIGGER ) {
        frame_ready = 1;
        //P2IES ^= I2C_MASTER_TRIGGER; // Toggle Edge sensitivity
        P2IFG &= ~I2C_MASTER_TRIGGER;  // Clear Interrupt Flag
    }
}

void ProcessCommand() {
    P1OUT ^= BIT0;
    if(frame_ready == 1){
        if (more_data_to_send == 0){
            read_from_i2c();
            pending_command = packet_received.payload[0];
        }
        frame_ready = 0;
        // Data is stored in struct FrameData packet_received

        /* TODO: Procesar comandos - Ponce*/
        /* Si es necesario mandar una respuesta, :
         *  - Almacenarla en struct FrameData packet_to_send (ya es una variable global)
         *  - Levantar la bandera data_pending_to_send a 1
         *  - el ultimo byte de packet_to_send.payload debe ser igual a 0xFF
         *    para que el otro micro sepa cuando ya tiene toda la informacion
         *  - almacenar la cantidad de bytes que hay en packet_to_send.payload en
         *    la variable global amount_of_data_to_send
         */

        if (rx_amount != 0){
            if (pending_command > 0 && pending_command < 30) { // Health status
                SendTelemetry();
                more_data_to_send = 0;
                pending_command = 0;
                storeInLog("CommHealth");
            }
            else if (pending_command >= 30 && pending_command < 60) { // Frequency change
                // TODO Frequency change
                uint32_t* freq_change = (uint32_t*)&packet_received.payload[1];
                if (*freq_change > 1000){
                    changeTaskPeriod(BEACON_TASK_ID, *freq_change);
                    storeInLog("FreqChange");
                } else{
                    storeInLog("NoFreqChange");
                }
            }
            else if (pending_command >= 60 && pending_command < 90) { // Get Logs of period
                storeInLog("CommGetLogs");
                get_logs_of_period();
            }
            else if (pending_command >= 90 && pending_command < 120) { // Transmit from memory
                storeInLog("CommGetMemory");
                transmit_memory();
            }
            else if (pending_command >= 120 && pending_command < 150) { // Overwrite memory
                storeInLog("CommOvwMemory");
                overwrite_memory();
            }
            else if (pending_command >= 150 && pending_command < 180) { // Clean memory
                storeInLog("CommCleanMemory");
                clean_memory();
            }
            else if (pending_command >= 180 && pending_command < 210) { // Clean logs
                clean_logs();
                more_data_to_send = 0;
                pending_command = 0;
                storeInLog("CommCleanLogs");
            }
        }
    }
}

void read_from_i2c(){
    uint8_t tx_data = 0xFE;

    struct i2c_data data = i2c_build_bundle((uint8_t *)&tx_data, 1, (uint8_t *)&rx_amount, 1);
    if( i2c_transfer(COMMS_HANDLER_ADDRESS, &data) == 0 ) {
        struct i2c_data result = i2c_build_bundle((uint8_t *)&tx_data, 0, (uint8_t *)&packet_received, rx_amount);
        i2c_transfer(COMMS_HANDLER_ADDRESS, &result);
    }
}

void createLog(char* message){
    new_log = EmptyLog;
    log_msg_length = 0;
    // Get datetime to store in log
    // 7 bytes for datetime + 1 string length + 1 variable string length
    if (getDatetime(&datetime) == 0) {
        new_log.datetime = datetime;
        new_log.message = message;
        while(*message++){
            log_msg_length++;
        }
        new_log.message_length = log_msg_length;
    }
}

void storeInLog(char* message){
    // TODO write into memory
    // Momentarily writing into 5 space array
    createLog(message);
    logs[log_counter] = new_log;
    log_counter++;
    logs_created++;
    if (log_counter == MAX_LOGS){
        log_counter = 0;
    }
}

void check_logs(log* checked_logs){
    // TODO read from memory's last 3 logs
    // Momentarily reading from last 3 array spaces
    check_logs_i = 0;
    if (logs_created <= 3){
        while (check_logs_i < logs_created){
            checked_logs[check_logs_i] = logs[check_logs_i];
            check_logs_i++;
        }
        log_logs_to_check = logs_created;
    } else if (logs_created >= MAX_LOGS){
        while (check_logs_i < 3){
            checked_logs[check_logs_i] = logs[MAX_LOGS-(check_logs_i+1)];
            check_logs_i++;
        }
        log_logs_to_check = 3;
    } else{
        while (check_logs_i < 3){
            checked_logs[check_logs_i] = logs[logs_created-(check_logs_i+1)];
            check_logs_i++;
        }
        log_logs_to_check = 3;
    }
}

void clean_logs(){
    clean_logs_i = 0;
    while(clean_logs_i < MAX_LOGS){
        logs[clean_logs_i] = EmptyLog;
        clean_logs_i++;
    }
    log_counter = 0;
    logs_created = 0;
}

void get_logs_of_period(){
    if (more_data_to_send == 0){
        log_logs_to_check = 0;
        check_logs(send_logs);
        log_i = 0;
        log_index = 0;
        log_struct_pos = 0;
    }
    log_payload_count = 0;
    *(packet_to_send.address) = *(packet_received.address);

    // Check if we still have room in the payload
    while (log_payload_count < (PAYLOAD_MAX_SIZE-1)){
        // Check if there is more logs to transmit
        if (log_i == log_logs_to_check){
            break;
        }

        // Write logs into payload
        if (log_struct_pos == 0){
            packet_to_send.payload[log_payload_count] = send_logs[log_i].datetime.seconds;
        } else if (log_struct_pos == 1){
            packet_to_send.payload[log_payload_count] = send_logs[log_i].datetime.minutes;
        } else if (log_struct_pos == 2){
            packet_to_send.payload[log_payload_count] = send_logs[log_i].datetime.hours;
        } else if (log_struct_pos == 3){
            packet_to_send.payload[log_payload_count] = send_logs[log_i].datetime.dow;
        } else if (log_struct_pos == 4){
            packet_to_send.payload[log_payload_count] = send_logs[log_i].datetime.day;
        } else if (log_struct_pos == 5){
            packet_to_send.payload[log_payload_count] = send_logs[log_i].datetime.month;
        } else if (log_struct_pos == 6){
            packet_to_send.payload[log_payload_count] = send_logs[log_i].datetime.year;
        } else if (log_struct_pos == 7){
            packet_to_send.payload[log_payload_count] = send_logs[log_i].message_length;
        } else{
            if ((log_struct_pos - 8) < send_logs[log_i].message_length){
                packet_to_send.payload[log_payload_count] = send_logs[log_i].message[log_struct_pos - 8];
            }
        }
        log_payload_count++;
        log_struct_pos++;

        // We ended with one log, moving on to next one
        if (log_struct_pos == (uint8_t)8 + send_logs[log_i].message_length){
            log_struct_pos = 0;
            log_i++;
        }
    }

    // Check if we finished sending the logs
    if (log_i == log_logs_to_check){
        more_data_to_send = 0;
        pending_command = 0;
    } else{
        more_data_to_send = 1;
    }

    // Sending the data, filled with logs
    packet_to_send.index = log_index;
    log_index++;
    packet_to_send.payload[log_payload_count] = 0XFF;
    amount_of_data_to_send = log_payload_count + 1;
    data_pending_to_send = 1;
}

void overwrite_memory(){
    *(packet_to_send.address) = *(packet_received.address);
    overwrite_memory_i = 0;
    overwrite_payload_count = 1;

    // If it is not the first part of some data
    if (packet_received.index > 0){
        // Searches for the last byte of utilized memory marked by 0xFF
        while (memory[overwrite_memory_i] != 0XFF){
            overwrite_memory_i++;
        }
    }

    // Write into memory the data from payload
    while(overwrite_payload_count < (rx_amount - 8)){
        if (overwrite_memory_i == (MEMORY_SIZE-1)){
            break;
        }
        memory[overwrite_memory_i] = packet_received.payload[overwrite_payload_count];
        overwrite_memory_i++;
        overwrite_payload_count++;
    }
    // Write the marker at the finished position
    memory[overwrite_memory_i] = 0xFF;

    // If we didnt finish to write all the payload into memory
    if (overwrite_payload_count != (rx_amount-8)){
        // Send an error packet
        packet_to_send.index = 0;
        // The combination of this 2 bytes will mean not enough memory
        packet_to_send.payload[0] = 0xFE;
        packet_to_send.payload[1] = 0x01;
        packet_to_send.payload[2] = 0xFF;
        amount_of_data_to_send = 3;
        more_data_to_send = 0;
        pending_command = 0;
        data_pending_to_send = 1;
    }
}

void transmit_memory(){
    if (more_data_to_send == 0){
        transmit_memory_i = 0;
        transmit_memory_index = 0;
    }
    transmit_payload_count = 0;
    uint8_t i;
    for (i=0; i<7; i++){
        packet_to_send.address[i] = packet_received.address[i];
    }

    // Check if there is still space in the payload
    while (transmit_payload_count < (PAYLOAD_MAX_SIZE-1)){
        // Check if we have sending the memory
        if(memory[transmit_memory_i] == 0XFF){
            break;
        }

        // Assign to paylod a byte from memory
        packet_to_send.payload[transmit_payload_count] = memory[transmit_memory_i];
        transmit_payload_count++;
        transmit_memory_i++;
    }

    if(memory[transmit_memory_i] == 0XFF){
        more_data_to_send = 0;
        pending_command = 0;
    } else {
        more_data_to_send = 1;
    }

    // Checking if memory has something
    if (transmit_memory_i == 0){
        // The combination of this 2 bytes will mean empty memory
        packet_to_send.payload[0] = 0xFE;
        packet_to_send.payload[1] = 0x00;
        packet_to_send.payload[2] = 0XFF;
        amount_of_data_to_send = 3;
        packet_to_send.index = 0;
        data_pending_to_send = 1;
    } else{
        // Sending the data, filled with the memory
        packet_to_send.index = transmit_memory_index;
        transmit_memory_index++;
        packet_to_send.payload[transmit_payload_count] = 0XFF;
        amount_of_data_to_send = transmit_payload_count + 1;
        data_pending_to_send = 1;
    }
}

void clean_memory(){
    clean_mem_i = 0;
    // Puts all of the memory to zero
    while(clean_mem_i < MEMORY_SIZE){
        memory[clean_mem_i] = 0;
        clean_mem_i++;
    }
    memory[0] = 0xFF;
    more_data_to_send = 0;
    pending_command = 0;
}

void SendPendingData() {
    if( data_pending_to_send == 1 && (P2IN & TRANSMISSION_READY)) {
        // Send data stored in packet_to_send
        struct i2c_data data = i2c_build_bundle((uint8_t *)&packet_to_send, 8+amount_of_data_to_send, NULL, 0);
        if( i2c_transfer(COMMS_HANDLER_ADDRESS, &data) == 0 ) {
            // El otro micro ya tiene la trama y la va a mandar
            data_pending_to_send = 0;
            if (more_data_to_send == 1){
                if (pending_command >= 60 && pending_command < 90) { // Get last 3 logs
                    get_logs_of_period();
                } else if (pending_command >= 90 && pending_command < 120) { // Transmit from memory
                    transmit_memory();
                }
            }
        }
    }
}

void CheckPowerManagement() {
    // TODO
}

void SendTelemetry() {
    storeInLog("CheckSensors");
    /* Only send telemetry if there is no pending data to be sent */
    if(data_pending_to_send == 0 && (P2IN & TRANSMISSION_READY)) {
        // Transmit telemetry
        if( BMP180_requestTemperature() == 0 && getDatetime((struct DS3231_DATETIME*)&packet_to_send.payload) == 0 ) {
            // Build telemetry package
            packet_to_send.index = 0;
            *(packet_to_send.address) = 0x00000000000000;
            int16_t temperature = BMP180_computeTemperature(bmp180Calibrated, BMP180_readRawTemperature());
            packet_to_send.payload[7] = (uint8_t)(temperature >> 8);
            packet_to_send.payload[8] = (uint8_t)(temperature & 0xFF);
            packet_to_send.payload[9] = 0xFF;
            // Send
            struct i2c_data data = i2c_build_bundle((uint8_t *)&packet_to_send, 18, NULL, 0);
            i2c_transfer(0x48, &data);
        }
    }
}
