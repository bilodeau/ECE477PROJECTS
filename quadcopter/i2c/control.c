#include <avr/io.h>
#include "../AVRserial.h"
#include <string.h>
#include "magnetometer.h"

void forward_command();

int main(){
	setup_serial();
        setup_i2c();
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
        if (!strcmp(receive_buffer,"M")){
                query_magnetometer();
        }else if (!strcmp(receive_buffer,"P")){
               power_on_magnetometer();
        }else if(!strcmp(receive_buffer,"S")){
                get_status_register();
        }   
}

