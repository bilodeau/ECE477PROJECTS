#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include "pidcontrol.h"
#include "../lib/data.h"
#include <stdio.h>
#include "../lib/AVRserial.h"
#include <avr/io.h>

#define ALT_P_GAIN 1
#define ALT_I_GAIN 1
#define ALT_D_GAIN 1

#define ROLL_P_GAIN 1
#define ROLL_I_GAIN 0
#define ROLL_D_GAIN 1

#define PITCH_P_GAIN 1
#define PITCH_I_GAIN 0
#define PITCH_D_GAIN 1

char controller_north_thrust;
char controller_south_thrust;
char controller_east_thrust;
char controller_west_thrust;

struct pid_values alt_control, roll_control, pitch_control;
struct goal_attrib target_state;

void query_cntrl_vals() {
        char temp[60];
	sprintf(temp, "Kp: %d, Ki: %d, Kp: %d",(int)(alt_control.Kp*100), (int)(alt_control.Ki*100), (int)(alt_control.Kd*100));
	transmit(temp);
}

void set_controller_p(float i){
	alt_control.Kp = i;
}

void set_controller_i(float i) {
        alt_control.Ki = i;
}

void set_controller_d(float i) {
        alt_control.Kd = i;
}

void setup_target_state(){
	target_state.altitude = 0;
	target_state.pitch = 0;
	target_state.roll = 0;
	target_state.yaw = 0;
}

void set_altitude(int alt){
	target_state.altitude = alt;
}

void setup_controller(){
	setup_target_state();
	int time = TCNT1;
	set_attrib(&alt_control, ALT_P_GAIN, ALT_I_GAIN, ALT_D_GAIN, 0, time);
	set_attrib(&roll_control, ROLL_P_GAIN, ROLL_I_GAIN, ROLL_D_GAIN, 0, time);
	set_attrib(&pitch_control, PITCH_P_GAIN, PITCH_I_GAIN, PITCH_D_GAIN, 0, time);	
}

void compute_controller(){
	int time = TCNT1;
	float alt_gain = get_gain(&alt_control, target_state.altitude, sensor_data_cache.sonar_distance, time);
	float roll_gain = get_gain(&roll_control, target_state.roll, sensor_data_cache.roll, time);
	float pitch_gain = get_gain(&pitch_control, target_state.pitch, sensor_data_cache.pitch, time);
	controller_north_thrust += alt_gain + pitch_gain; 
	controller_south_thrust += alt_gain - pitch_gain;
	controller_east_thrust += alt_gain + roll_gain;
	controller_west_thrust += alt_gain - roll_gain;
	if (controller_north_thrust > 100) {
		controller_north_thrust = 100;
	} else if (controller_north_thrust < 6) {
		controller_north_thrust = 6;
	}
	if (controller_south_thrust > 100) {
		controller_south_thrust = 100;
	} else if (controller_south_thrust < 6) {
		controller_south_thrust = 6;
	}
	if (controller_east_thrust > 100) {
		controller_east_thrust = 100;
	} else if (controller_east_thrust < 6) {
		controller_east_thrust = 6;
	}
	if (controller_west_thrust > 100) {
		controller_west_thrust = 100;
	} else if (controller _west_thrust < 6) {
		controller_west_thrust = 6;
	}
}

char get_north_thrust(){
	return controller_north_thrust;
}

char get_south_thrust(){
	return controller_south_thrust;
}

char get_east_thrust(){
	return controller_east_thrust;
}

char get_west_thrust(){
	return controller_west_thrust;
}
#endif
