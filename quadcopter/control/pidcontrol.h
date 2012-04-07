#ifndef PIDCONTROL_H_
#define PIDCONTROL_H_

struct pid_values{
int Kp; //constants for the pid controllers
int Ki;
int Kd;
float old_error; //the old error value
float old_time;	//time between readings
};


struct goal_attrib{ //what we want all of the sensor data to be to have a certain mode. 
int yaw;
int pitch;
int altitude;
int roll;
};

struct attribute_control{//
	struct pid values; //{Kp,Ki,Kd,old,error,delta_T,goal,measure}
	struct quad_attributes;//{yaw,pitch,sonar,roll}
};

void query_controller_constants() {
	char *temp = malloc(60);
	sprintf(temp, "Kp: %d, Ki: %d, Kp: %d", alt_control.Kp*100, alt_control.Ki*100, alt_control.Kd*100);
	transmit(temp);
}

void set_controller_p(float i) {
	alt_controller.Kp = i;
}

void set_controller_i(float i) {
	alt_controller.Ki = i;
}

void set_controller_d(float i) {
	alt_controller.Kd = i;
}

void set_attrib(struct pid_values *s, float p, float i, float d, int error, int time) {
	s->Kp = p;
	s->Ki = i;
	s->Kd = d;
	s->old_error = error;
	s->old_time = time;
}

float get_gain(struct pid_control *s, float goal, float actual, int time){
	int delta_t = time - s->old_time;
	if (delta_t < 0) {
		delta_t = 0xFFFF - old_time + time + 1; 
	}
	s->old_time = time;
	int error = get_diff(goal, actual);
	return (s->Kp)*error + (s->Ki)*get_integral(s, error, delta_t) + (s->Kd)*get_deriv(s, error, delta_t);
}

//calculates the error
int get_diff(int goal, int measure){
	return goal - measure;
}

//calculates the integral of error
float get_integral(struct pid_values *s, int error, int delta_t){
	float integral = delta_t * ((s->old_error + error) / 2.);		
	return integral;
}

//calculates the derivative error
float get_deriv(struct pid_values *s, int error, int delta_t){
	float derivative = (error - s->old_error) / ((float) (delta_T));
	s->old_error = error;
	return derivative;
}
#endif
