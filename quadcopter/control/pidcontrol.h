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
	s->accum_error = 0;
	s->old_error = 0;
}

//calculates the error
int get_error(int goal, int measure){
	return goal - measure;
}

//calculates the integral of error
int get_integral(struct pid_values *s, int error){
	s->accum_error += error;		
	return s->accum_error;
}

//calculates the derivative error
int get_deriv(struct pid_values *s, int error){
	int derivative = (error - s->old_error);
	s->old_error = error;
	return derivative;
}

int get_gain(struct pid_values *s, int goal, int actual){
	int error = get_error(goal, actual);
	return (s->Kp)/1000.*error + (s->Ki)/10000.*get_integral(s, error) + (s->Kd)*get_deriv(s, error);
}
#endif
