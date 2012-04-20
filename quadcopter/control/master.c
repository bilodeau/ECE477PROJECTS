#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "../lib/AVRserial.h"
#include "../lib/delay.h"
#include "../lib/devices/spam.h"
#include "../lib/devices/poll_devices.h"
#include "controller.h"
#include "../motors/motors.h"
#include "takeoff.h"
#include "UI.h"

void setup_spam();
char TOP_flag = 0;
char compare_A_flag = 0;
char spam_flag = 0;

// mode variables declared in UI.h
//char begin = 0; // begin mode hands off the motors to the PD controllers
//char idle = 0; // idle mode just spins the motors for checking operation and communication
int main(){
	setup_serial();
	setup_spam();
	setup_i2c();
	setup_motors();
        setup_controller();
	setup_takeoff();
	serial_command_ready = 0;
        while(1){
		if (serial_command_ready){
			forward_command();
                        serial_command_ready = 0;
                }
		if (TOP_flag){ // 30.5Hz
			TOP_flag = 0;
			poll_sonar();
			update_adj_alt();
			if(!spam_flag){
				send_spam();
			}
			spam_flag++;
			if (spam_flag > 6)
				spam_flag = 0;
			update_takeoff_and_landing();
		}
		if (compare_A_flag){ // 122HZ
			compare_A_flag = 0;
			poll_gyro();
			poll_magnetometer();
			update_adj_yaw(); // update the filtered yaw value
			poll_nunchuck();
			poll_angles();
			poll_gyro();
			update_adj_rp(); // update the filtered roll and pitch values
			if (begin){
				update_motors();
			}else if(idle){
				setup_takeoff();
				idle_motors();
			}else{
				setup_takeoff();
				stop_motors();
        		}
		}
	}   
        return 0;
}

// sets up Timer1 for sending data at regular intervals
void setup_spam(){
	TCCR1A = 0; // normal port operation, CTC, ICR1 is TOP
	TCCR1B = (1<<WGM13)|(1<<WGM12)|2; // WGM mode 12, use prescaler 1/8, counts in us
	ICR1 = 0x7FFF; // TOP value, clears every 32.768ms, 30.5Hz
	OCR1A = 0x2000; // interrupt every 8.192ms, 122Hz
	TIMSK1 = (1<<ICIE1)|(1<<OCIE1A); // enable TOP interrupt, and compare match for A
	sei(); // enable global interrupts
}

ISR(TIMER1_CAPT_vect){
	TOP_flag = 1;
}

ISR(TIMER1_COMPA_vect){
	compare_A_flag = 1;
	OCR1A += 0x2000;
	if(OCR1A > ICR1)
		OCR1A -= ICR1;
}
