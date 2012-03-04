#include <avr/io.h>
#include "../lib/AVRserial.h"
#include <string.h>
#include "magnetometer.h"
#include "barometer.h"
#include "../lib/sonar.h"

void forward_command();

int main(){
	setup_delay();
	setup_serial();
        setup_i2c();
	setup_sonar();
        serial_command_ready = 0;
        while(1){
		if (serial_command_ready){
			forward_command();
                        serial_command_ready = 0;
                }   
        }   
        return 0;
}

// issues the command in serial_command_buffer to the i2c bus
void forward_command(){
        if (!strcmp(receive_buffer,"MD")){
                query_magnetometer();
        }else if (!strcmp(receive_buffer,"MP")){
               power_on_magnetometer();
        }else if(!strcmp(receive_buffer,"MS")){
                get_magnetometer_status();
        }else if(!strcmp(receive_buffer,"BC")){
		get_barometer_calibration();
	}else if(!strcmp(receive_buffer,"BD")){
		query_barometer();
	}else if(!strcmp(receive_buffer,"BT")){
		query_barometer_true();
	}else if(!strcmp(receive_buffer,"SONAR")){
		query_sonar();
	}
}

