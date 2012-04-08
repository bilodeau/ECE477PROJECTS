#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include "pidcontrol.h"
#include "../lib/data.h"
#include <stdio.h>
#include "../lib/AVRserial.h"
#include <avr/io.h>

#define ALT_P_GAIN 1
#define ALT_I_GAIN 0
#define ALT_D_GAIN 0

#define ROLL_P_GAIN 0
#define ROLL_I_GAIN 0
#define ROLL_D_GAIN 0

#define PITCH_P_GAIN 0
#define PITCH_I_GAIN 0
#define PITCH_D_GAIN 0

int base_thrust;

long controller_north_thrust;
long controller_south_thrust;
long controller_east_thrust;
long controller_west_thrust;

struct pid_values alt_control, roll_control, pitch_control;
struct goal_attrib target_state;

void query_cntrl_vals() {
        char temp[60];
	sprintf(temp, "Kp: %d, Ki: %d, Kp: %d",alt_control.Kp*100, alt_control.Ki*100, alt_control.Kd*100);
	transmit(temp);
}

void set_controller_p(float i){
        alt_control.accum_error = 0;
	alt_control.Kp = i;
}

void set_controller_i(float i) {
        alt_control.accum_error = 0;
	alt_control.Ki = i;
}

void set_controller_d(float i) {
        alt_control.accum_error = 0;
        alt_control.Kd = i;
}

void setup_target_state(){
	target_state.altitude = 0;
	target_state.pitch = 0;
	target_state.roll = 0;
	target_state.yaw = 0;
}

void set_base_thrust(int i){
	base_thrust = i;
	char temp[40];
	sprintf(temp,"new base: %d",base_thrust);
	transmit(temp);
}

void set_altitude(int alt){
	target_state.altitude = alt;
	char temp[40];
	sprintf(temp,"new alt: %d",target_state.altitude);
	transmit(temp);
}

void setup_controller(){
	setup_target_state();
	set_attrib(&alt_control, ALT_P_GAIN, ALT_I_GAIN, ALT_D_GAIN);
	set_attrib(&roll_control, ROLL_P_GAIN, ROLL_I_GAIN, ROLL_D_GAIN);
	set_attrib(&pitch_control, PITCH_P_GAIN, PITCH_I_GAIN, PITCH_D_GAIN);	
}

void compute_controller(){
	int alt_gain = get_gain(&alt_control, target_state.altitude, sensor_data_cache.sonar_distance);
	int roll_gain = get_gain(&roll_control, target_state.roll, sensor_data_cache.roll);
	int pitch_gain = get_gain(&pitch_control, target_state.pitch, sensor_data_cache.pitch);
	
	controller_north_thrust = base_thrust + alt_gain + pitch_gain; 
	controller_south_thrust = base_thrust + alt_gain - pitch_gain;
	controller_east_thrust = base_thrust + alt_gain + roll_gain;
	controller_west_thrust = base_thrust + alt_gain - roll_gain;
	if (controller_north_thrust > 100) {
		controller_north_thrust = 100;
	} else if (controller_north_thrust < 10) {
		controller_north_thrust = 10;
	}
	if (controller_south_thrust > 100) {
		controller_south_thrust = 100;
	} else if (controller_south_thrust < 10) {
		controller_south_thrust = 10;
	}
	if (controller_east_thrust > 100) {
		controller_east_thrust = 100;
	} else if (controller_east_thrust < 10) {
		controller_east_thrust = 10;
	}
	if (controller_west_thrust > 100) {
		controller_west_thrust = 100;
	} else if (controller_west_thrust < 10) {
		controller_west_thrust = 10;
	}
	char temp[100];
	sprintf(temp,"power: %d gain: %d",controller_north_thrust,alt_gain*100);
	transmit(temp);
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
