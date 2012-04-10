#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "../lib/AVRserial.h"
#include "../lib/delay.h"
#include "../lib/devices/spam.h"
#include "../lib/devices/poll_devices.h"
#include "controller.h"
#include "../motors/motors.h"

void forward_command();
void setup_spam();
void set_calibs();
char overflow_flag = 0;
char compare_A_flag = 0;

char begin = 0; // begin mode hands off the motors to the PD controllers
char idle = 0; // idle mode just spins the motors for checking operation and communication
int main(){
	setup_serial();
	setup_spam();
	setup_i2c();
	setup_motors();
        setup_controller();
	serial_command_ready = 0;
        while(1){
		if (serial_command_ready){
			forward_command();
                        serial_command_ready = 0;
                }
		if (overflow_flag){ // 30.5Hz
			overflow_flag = 0;
			poll_sonar();
			update_adj_alt();
			send_spam();
		}
		if (compare_A_flag){ // 122HZ
			compare_A_flag = 0;
			poll_gyro();
			poll_nunchuck();
			poll_angles();
			poll_gyro();
			update_adj_rp(); // update the filtered roll and pitch values
			poll_magnetometer();
			update_adj_yaw(); // update the filtered yaw value
			if (begin){
				update_motors();
			}else if(idle){
				idle_motors();
			}else{
				stop_motors();
        		}
		}
	}   
        return 0;
}

void forward_command(){
	if(!strcmp(receive_buffer,"BOOT")){
	// the delays here are necessary to allow each device to finish booting
	// otherwise we run into some problems with the i2c bus
		power_on_gyro();
		delay(10);
		power_on_magnetometer();
		delay(10);
		//hard_barometer_calibration();
		//delay(10);
		power_on_nunchuck();
	}else if(!strcmp(receive_buffer,"BEGIN")){
		begin = 1;
	}else if(!strcmp(receive_buffer,"STOP")){
		begin = 0;
		idle = 0;
	}else if(!strcmp(receive_buffer,"IDLE")){
		idle = 1;
	}else if(!strcmp(receive_buffer,"GP")){
		power_on_gyro();
        }else if (!strcmp(receive_buffer,"MP")){
               power_on_magnetometer();
        }else if(!strcmp(receive_buffer,"MS")){
                get_magnetometer_status();
        }else if(!strcmp(receive_buffer,"BC")){
	       	hard_barometer_calibration();
	}else if(!strcmp(receive_buffer,"BT")){
		query_barometer_true();
	}else if(!strcmp(receive_buffer,"NP")){
		power_on_nunchuck();
	}else if(!strcmp(receive_buffer,"ND")){
		query_nunchuck();
	}else if(!strcmp(receive_buffer,"NZ")){
		zero_nunchuck();
	}else if(!strcmp(receive_buffer,"FETCH")){
		query_cntrl_vals();
	}else if(!strncmp(receive_buffer,"SET",3)){
		set_calibs();
	}else if(!strncmp(receive_buffer,"HOVER ",6)){
		int alt;
		sscanf(receive_buffer+6,"%d",&alt);
		if ((alt >= 0)&&(alt <= 400))
			set_altitude(alt);
	}else if(!strncmp(receive_buffer,"POWER ",6)){
		int power;
		sscanf(receive_buffer+6,"%d",&power);
		set_base_thrust(power);
	}
}

void set_calibs(){
		int calib_value = 0;
		sscanf(receive_buffer+5,"%d",&calib_value);
		if(receive_buffer[3] == 'P'){
			set_controller_p(calib_value);
		}else if(receive_buffer[3] == 'D'){
			set_controller_d(calib_value);
		}	
}

// sets up Timer1 for sending data at regular intervals
void setup_spam(){
	TCCR1A = 0; // normal port operation, CTC, ICR1 is TOP
	TCCR1B = (1<<WGM13)|(1<<WGM12)|2; // WGM mode 12, use prescaler 1/8, counts in us
	ICR1 = 0x7FFF; // TOP value, overflows every 32.768ms, 30.5Hz
	OCR1A = 0x2000; // interrupt every 8.192ms, 122Hz
	TIMSK1 = (1<<TOIE1)|(1<<OCIE1A)|(1<<OCIE1B); // enable overflow interrupt, and compare match for A and B
	sei(); // enable global interrupts
}

ISR(TIMER1_OVF_vect){
	overflow_flag = 1;
}

ISR(TIMER1_COMPA_vect){
	compare_A_flag = 1;
	OCR1A += 0x2000;
}
