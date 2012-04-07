#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "../lib/AVRserial.h"
#include "../lib/delay.h"
#include "../i2c/magnetometer.h"
#include "../i2c/barometer.h"
#include "../i2c/nunchuck.h"
#include "../i2c/gyro.h"
#include "../i2c/spam.h"
#include "../motors/motors.h"

void forward_command();
void setup_spam();
void set_calibs();
char spam_flag = 0;
char update_motors_flag = 0;
char begin = 0;
char idle = 0;
int main(){
	setup_spam();
	setup_serial();
        setup_i2c();
	setup_motors();
        setup_controller();
	serial_command_ready = 0;
        while(1){
		if (serial_command_ready){
			forward_command();
                        serial_command_ready = 0;
                }
		if (spam_flag){
			send_spam();
			spam_flag = 0;
		}
		if (update_motors_flag && begin){
			update_motors();
			update_motors_flag = 0;  
		}else if(idle){
			set_motors(10);
		}else{
			stop_motors();
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
		hard_barometer_calibration();
		delay(10);
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
		set_altitude(alt);
	}
}

void set_calibs(){
		int calib_value = 0;
		sscanf(receive_buffer+5,"%d",&calib_value);
		float calib_valf = calib_value / 100.;
		if(receive_buffer[3] == 'P'){
			set_controller_p(calib_valf);
		}else if(receive_buffer[3] == 'I'){
			set_controller_i(calib_valf);
		}else if(receive_buffer[3] == 'D'){
			set_controller_d(calib_valf);
		}	
}

// sets up Timer1 for sending data at regular intervals
void setup_spam(){
	TCCR1A = 0; // use WGM mode 4, CTC, OCR1A is TOP
	TCCR1B = 10; // normal port operation, use prescaler 1/8, counts in us
	OCR1A = 0x7FFF;// TOP value, so overflows every 32.768ms
	TIMSK1 = 2; // enable overflow interrupt
	sei();	
}

ISR(TIMER1_COMPA_vect){
	spam_flag = 1;
	update_motors_flag = 1;
}
