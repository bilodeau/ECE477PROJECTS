#ifndef TAKEOFF_H_
#define TAKEOFF_H_

#define TAKEOFFTHRUST 40
#define TAKEOFFINTERRUPTCOUNTERLIMIT 300 // interrupt fires at 30Hz so this is about 10 seconds

int takeoff_interrupt_counter;
int takeoff;
// int base_thrust; // declared in controller.h
void setup_takeoff(){
	takeoff = 0;
	base_thrust = 0;
	takeoff_interrupt_counter = 0;
}

void begin_takeoff(){
	takeoff = 1;
}

void update_takeoff(){
	if(takeoff_interrupt_counter < TAKEOFFINTERRUPTCOUNTERLIMIT)
		takeoff_interrupt_counter++;
	if ((takeoff)&&(base_thrust < TAKEOFFTHRUST)){
		base_thrust = TAKEOFFTHRUST * takeoff_interrupt_counter / TAKEOFFINTERRUPTCOUNTERLIMIT;
	}
	if((!takeoff)&&(base_thrust > 0)){
		base_thrust = TAKEOFFTHRUST * (TAKEOFFINTERRUPTCOUNTERLIMIT - takeoff_interrupt_counter)/TAKEOFFINTERRUPTCOUNTERLIMIT;
	}
}
#endif
