#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include <avr/io.h>
#include <stdio.h>
#include "../lib/AVRserial.h"
#include "../lib/data.h"
#include "pidcontrol.h"
#include "../motors/motors.h"

#define ALT_P_GAIN 1
#define ALT_D_GAIN 0
#define ALT_GAIN_LIMIT 0

#define FLIP_P_GAIN 0
#define FLIP_D_GAIN 0
#define FLIP_GAIN_LIMIT 0

int base_thrust;

long controller_north_thrust;
long controller_south_thrust;
long controller_east_thrust;
long controller_west_thrust;

struct pid_values alt_control, roll_control, pitch_control;
struct goal_attrib target_state;

void query_cntrl_vals() {
        char temp[60];
	sprintf(temp, "Kp: %d, Kd: %d",alt_control.Kp, alt_control.Kd);
	transmit(temp);
}

void set_controller_p(int i){
        alt_control.Kp = i;
}

void set_controller_d(int i) {
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
	set_attrib(&alt_control, ALT_P_GAIN, ALT_D_GAIN, ALT_GAIN_LIMIT);
	set_attrib(&roll_control, FLIP_P_GAIN, FLIP_D_GAIN, FLIP_GAIN_LIMIT);
	set_attrib(&pitch_control, FLIP_P_GAIN, FLIP_D_GAIN, FLIP_GAIN_LIMIT);	
}

void compute_controller(){
	int alt_gain = get_gain(&alt_control, target_state.altitude, sensor_data_cache.filt_altitude);
	int roll_gain = get_gain(&roll_control, target_state.roll, sensor_data_cache.filt_roll_angle);
	int pitch_gain = get_gain(&pitch_control, target_state.pitch, sensor_data_cache.filt_pitch_angle);
	
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
	sprintf(temp,"power: %d gain: %d",(int)controller_north_thrust,alt_gain);
	transmit(temp);
}

void update_motors(){
	compute_controller();
	set_motor_power(NORTHMOTOR,controller_north_thrust);
	set_motor_power(SOUTHMOTOR,controller_south_thrust);
	set_motor_power(EASTMOTOR,controller_east_thrust);
	set_motor_power(WESTMOTOR,controller_west_thrust);
}
#endif
