#ifndef TAKEOFF_H_
#define TAKEOFF_H_

#define MINTHRUST 78
#define TAKEOFFTHRUST	130//78 is min// lift off at 177 we use the sum of the two
#define TAKEOFFINTERRUPTCOUNTERLIMIT 150 // interrupt fires at 30Hz so this is about 5 seconds
#define LANDINGINTERRUPTCOUNTERLIMIT 75 // interrupt fires at 30Hz so this is about 2.5 seconds

int takeoff, landing; // controller mode flags
int takeoff_interrupt_counter;
int landing_interrupt_counter;
int takeoff_thrust = TAKEOFFTHRUST;
int landing_thrust = MINTHRUST;

// int base_thrust; // declared in controller.h
void setup_takeoff(){
	landing = 0;
	takeoff = 0;
	base_thrust = MINTHRUST;
	takeoff_interrupt_counter = 0;
	landing_interrupt_counter = 0;	
}

void begin_takeoff(){
	setup_takeoff();
	takeoff = 1;
}
void begin_landing(){
	landing_thrust = base_thrust - MINTHRUST;
	setup_takeoff();
	base_thrust = landing_thrust + MINTHRUST;
	landing = 1;
}

void update_takeoff_and_landing(){
	if (takeoff){
		if(takeoff_interrupt_counter < TAKEOFFINTERRUPTCOUNTERLIMIT)
			takeoff_interrupt_counter++;
		if (base_thrust < takeoff_thrust+MINTHRUST){
		base_thrust = MINTHRUST + takeoff_thrust * takeoff_interrupt_counter / TAKEOFFINTERRUPTCOUNTERLIMIT;
		}
	}
	if (landing){
		if(landing_interrupt_counter < LANDINGINTERRUPTCOUNTERLIMIT)
			landing_interrupt_counter++;
		if (base_thrust > MINTHRUST){
			base_thrust = MINTHRUST + landing_thrust * ((LANDINGINTERRUPTCOUNTERLIMIT - landing_interrupt_counter))/ LANDINGINTERRUPTCOUNTERLIMIT;
		}
	}
}
#endif
