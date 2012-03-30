//

struct pid_values{
int Kp; //constants for the pid controllers
int Ki;
int Kd;
float old_error; //the old error value
float error;  //most recent error value
float delta_T;	//time between readings
float goal	//value for parameter we want
float measure	//values we are measuring for parameter
};


struct mode_attributes{ //what we want all of the sensor data to be to have a certain mode. 
int yaw;
int pitch;
int sonar;
int roll;
};

struct attribute_control{//
	struct pid values; //{Kp,Ki,Kd,old,error,delta_T,goal,measure}
	struct quad_attributes;//{yaw,pitch,sonar,roll}
}

void control(){
	control = Kp*error + Ki*integral + Kd*derivative;
}

//calculates the error
int get_error(goal,measure){
	error=goal-measure;
	return error;
}

//calculates the integral of error
int error_integral(error,delta_t){
	integral = delta_t*((old_error+error)/2);		
	old_error=error;
	return integral;
}

//calculates the derivative error
int get_derivative(error,delta_t){
	Derivative=(error-old_error)/delta_T;
	old_error=error;
	return derivative;
}
