#ifndef UI_H_
#define UI_H_

char begin = 0; // begin mode hands off the motors to the PD controllers
char idle = 0; // idle mode just spins the motors at minimum speed for checking operation and communication

void set_calibs(){
		int calib_value = 0;
		sscanf(receive_buffer+5,"%d",&calib_value);
		if(receive_buffer[3] == 'A'){
			if(receive_buffer[4] == 'P'){
				set_alt_controller_p(calib_value);
			}else if(receive_buffer[4] == 'D'){
				set_alt_controller_d(calib_value);
			}	
		}else if(receive_buffer[3] == 'F'){
			if(receive_buffer[4] == 'P'){
				set_flip_controller_p(calib_value);
			}else if(receive_buffer[4] == 'D'){
				set_flip_controller_d(calib_value);
			}	
		}
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
	}else if(!strcmp(receive_buffer,"LAND")){
		begin_landing();
	}else if(!strcmp(receive_buffer,"TAKEOFF")){
		begin_takeoff();
	}else if(!strncmp(receive_buffer,"TT ",3)){
		int thrust;
		sscanf(receive_buffer+3,"%d",&thrust);
		if ((thrust >= 10)&&(thrust <= 150))
			takeoff_thrust = thrust;
	}else if(!strcmp(receive_buffer,"GP")){
		power_on_gyro();
        }else if (!strcmp(receive_buffer,"MP")){
               power_on_magnetometer();
        }else if(!strcmp(receive_buffer,"MS")){
                get_magnetometer_status();
/*        }else if(!strcmp(receive_buffer,"BC")){
	       	hard_barometer_calibration();
	}else if(!strcmp(receive_buffer,"BT")){
		query_barometer_true();
*/	}else if(!strcmp(receive_buffer,"NP")){
		power_on_nunchuck();
	}else if(!strcmp(receive_buffer,"ND")){
		query_nunchuck();
	}else if(!strcmp(receive_buffer,"FETCH")){
		query_cntrl_vals();
	}else if(!strncmp(receive_buffer,"SET",3)){
		set_calibs();
	}else if(!strncmp(receive_buffer,"HOVER ",6)){
		int alt;
		sscanf(receive_buffer+6,"%d",&alt);
		if ((alt >= 0)&&(alt <= 1000))
			set_altitude(alt);
	}else if(!strncmp(receive_buffer,"OFFSET",6)){
		int offset;
		sscanf(receive_buffer+8,"%d",&offset);
		if(receive_buffer[6] == 'R'){
			roll_offset = offset;
		}else if(receive_buffer[6] == 'P'){
			pitch_offset = offset;
		}
		char temp[40];
		sprintf(temp,"r: %d  p: %d",roll_offset,pitch_offset);
		transmit(temp);
	}
}
#endif
