#include "controller.h"
#include "../lib/data.h"

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#define ALT_ADJ 5
#define ROLL_ADJ 5
#define PITCH_ADJ 5

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

void setup_controller(){
	struct pid_values alt_control, roll_control, pitch_control;
	int time = TCNT1;
	set_attrib(&alt_control, ALT_P_GAIN, ALT_I_GAIN, ALT_D_GAIN, 0, time);
	set_attrib(&roll_control, ROLL_P_GAIN, ROLL_I_GAIN, ROLL_D_GAIN, 0, time);
	set_attrib(&pitch_control, PITCH_P_GAIN, PITCH_I_GAIN, PICTH_D_GAIN, 0, time);	
	
}

void compute_controller(){
	int time = TCNT1;
	float alt_gain = get_gain(&alt_control, goal_attrib.altitude, sensor_data_cache.sonar_distance, time);
	float roll_gain = get_gain(&roll_control, goal_attrib.roll, sensor_data_cache.roll, time);
	float pitch_gain = get_gain(&pitch_control, goal_attrib.pitch, sensor_data_cache.pitch, time);
	controller_north_thrust += alt_gain*ALT_ADJ + pitch_gain*PITCH_ADJ; 
	controller_south_thrust += alt_gain*ALT_ADJ - pitch_gain*PITCH_ADJ
	controller_east_thrust += alt_gain*ALT_ADJ + roll_gain*ROLL_ADJ;
	conroller_west_thrust += alt_gain*ALT_ADJ - roll_gain*ROLL_ADJ;
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
