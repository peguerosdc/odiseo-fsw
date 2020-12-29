# Odiseo's Main Flight Software

Flight software of the main computer in charge of the power system, payload, sensors, and communication with the radio system.


## Features

* Task's scheduler
* The main computer is turned on after three hours after hypothetical deployment
* I2C ports enabled to communicate with [comms_computer](/comms_computer)

Five main tasks are performed by the main computer:

1. Process instructions received by the [comms_computer](/comms_computer). There are 7 instructions that can be  performed:
	* Send telemetry
	* Change beacon's frequency
	* Send logs
	* Send data stored in memory
	* Overwrite data stored in memory
	* Clean memory
	* Clean logs
2. Reply to instructions received by the [comms_computer](/comms_computer)
3. Check power management (**TODO**). This is meant to check if the batteries have enough energy to power the main operations of the satellite. If that's not the case, the backup battery is going to be used while the former is going to start charging.
4. Send telemetry every `BEACON_BASE_PERIOD` to notify the ground station that the satellite is alive. Temperature and pressure are sent as the beacon
5. Keep a log of the status of the satellite in the shaep of
	* Date and time
	* Status of the batteries
	* Telemetry

## Notes

Scheduler based on **[RIOS](https://www.ics.uci.edu/~givargis/pubs/C50.pdf)**

