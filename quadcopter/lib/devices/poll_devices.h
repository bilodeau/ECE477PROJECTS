#ifndef POLL_DEVICES_H_
#define POLL_DEVICES_H_
#include "spam.h"

void poll_magnetometer(){
	get_data_magnetometer();
}

void poll_sonar(){
	query_slave(0x00,4);
}

void poll_barometer(){
	get_data_barometer_true();
}

void poll_gyro(){
	trigger_gyro_conversion();
}

void poll_nunchuck(){
	get_data_nunchuck();
}

void poll_angles(){
	get_nunchuck_angles();
}
#endif
