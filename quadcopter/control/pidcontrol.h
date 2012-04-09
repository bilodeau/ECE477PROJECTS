#ifndef PIDCONTROL_H_
#define PIDCONTROL_H_
#include <stdlib.h>


struct pid_values{
int Kp; //constants for the pid controllers
int Ki;
int Kd;
long long accum_error;
int old_error; //the old error value
};


struct goal_attrib{ //what we want all of the sensor data to be to have a certain mode. 
int yaw;
int pitch;
int altitude;
int roll;
};

void set_attrib(struct pid_values *s, float p, float i, float d) {
	s->Kp = p;
	s->Ki = i;
	s->Kd = d;
	s->old_error = 0;
}

int get_adj_roll(struct sensor_data *s) {
	s->filt_roll_angle = (FILTER_C)*(s->filt_roll_angle + s->gyroscope.roll*DELTA_T) + (1 - FILTER_C)*(s->nunchuck_roll);
	return s->filt_roll_angle;
}

int get_adj_pitch(struct sensor_data *s) {
	s->filt_pitch_angle = (FILTER_C)*(s->filt_pitch_angle + s->gyroscope.pitch*DELTA_T) + (1 - FILTER_C)*(s->nunchuck_pitch);
	return s->filt_pitch_angle;
}

int get_adj_alt(struct sensor_data *s) {
	s->filt_alt = ALT_FILTER_C*(s->filt_alt) + (1 - ALT_FILTER_C)*(s->sonar_distance);
	return s->filt_alt;
}

int get_gain(struct pid_values *s, int goal, int actual){
	int error = goal - actual;
	int derivative = error - s->old_error;
	s->old_error = error;
	return (s->Kp)/1000.*error + (s->Kd)/1000.*derivative;
}
#endif
