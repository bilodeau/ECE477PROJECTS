#ifndef SPAM_H_
#define SPAM_H_
#include <string.h>
#include "../lib/data.h"
#include "magnetometer.h"
#include "barometer.h"
#include "gyro.h"
#include "nunchuck.h"

void spam(int sensor_code,long value){
	char temp[40];	
        sprintf(temp,"%c%c%ld",SENSORDATAPACKETCHARACTER,sensor_code,value);
	transmit(temp);
}
void spam_magnetometer(){
	get_data_magnetometer();
	spam(COMPASSHEADING,sensor_data_cache.compass_heading);
}

void spam_sonar(){
	query_slave(0x00,4);
	spam(SONARDISTANCE,sensor_data_cache.sonar_distance);
}

void spam_barometer(){
	get_data_barometer_true();
	spam(BAROMETERTEMPERATURE,sensor_data_cache.barometer_temperature);
	spam(BAROMETERPRESSURE,sensor_data_cache.barometer_pressure);
	spam(BAROMETERALTITUDE,sensor_data_cache.barometer_altitude);
}

void spam_gyro(){
	get_data_gyro();
	spam(GYROSCOPEY,sensor_data_cache.gyroscope_x_rotational_velocity);
	spam(GYROSCOPEX,sensor_data_cache.gyroscope_y_rotational_velocity);
	spam(GYROSCOPETEMPERATURE,sensor_data_cache.gyroscope_temperature);
}

void spam_nunchuck(){
	get_data_nunchuck();
	spam(NUNCHUCKX,sensor_data_cache.nunchuck_x_value);
	spam(NUNCHUCKY,sensor_data_cache.nunchuck_y_value);
	spam(NUNCHUCKZ,sensor_data_cache.nunchuck_z_value);
}

void spam_nunchuck_angles(){
	get_nunchuck_angles();
	spam(YAW,sensor_data_cache.yaw);
	spam(PITCH,sensor_data_cache.pitch);
	spam(ROLL,sensor_data_cache.roll);

}

void send_spam(){
	spam_magnetometer();
	spam_sonar();
	//spam_barometer();
	spam_gyro();
	spam_nunchuck();
	spam_nunchuck_angles();
}
#endif
