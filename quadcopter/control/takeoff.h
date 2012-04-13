#ifndef TAKEOFF_H_
#define TAKEOFF_H_

#define MINTHRUST 78
#define TAKEOFFTHRUST	20//102//78 is min// lift off at 177 we use the sum of the two
#define TAKEOFFINTERRUPTCOUNTERLIMIT 300 // interrupt fires at 30Hz so this is about 10 seconds

int takeoff_interrupt_counter;
int takeoff;

// int base_thrust; // declared in controller.h
void setup_takeoff(){
	takeoff = 0;
	base_thrust = MINTHRUST;
	takeoff_interrupt_counter = 0;
}

void begin_takeoff(){
	takeoff = 1;
}

void update_takeoff(){
	if(takeoff_interrupt_counter < TAKEOFFINTERRUPTCOUNTERLIMIT)
		takeoff_interrupt_counter++;
	if ((takeoff)&&(base_thrust < TAKEOFFTHRUST+MINTHRUST)){
		base_thrust = MINTHRUST+ TAKEOFFTHRUST * takeoff_interrupt_counter / TAKEOFFINTERRUPTCOUNTERLIMIT;
	}
	if((!takeoff)&&(base_thrust > MINTHRUST)){
		base_thrust = MINTHRUST + TAKEOFFTHRUST * (TAKEOFFINTERRUPTCOUNTERLIMIT - takeoff_interrupt_counter)/TAKEOFFINTERRUPTCOUNTERLIMIT;
	}
}
#endif
