#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "baud.h"
#include "data.h"

#define LOG 1// saves all commands into a log file
#define LOGSPAM 0
#define PRINTDEBUG 0
#define COMMANDBUFFERSIZE 81
#define SERIALPORTDEFAULT "/dev/tty.usbmodemfd1221"

void clear_serial_command_buffer();
void clear_user_command_buffer();
void setup_serial_port(char* serial_port_name);
void buffer_user_input();
void process_user_input();
void configure_user_input();
void transmit(char* c);
void read_from_serial_port();
void process_serial_command();
void log_serial_command();
void save_sensor_data_packet(char code);

struct termios originalsettings;
int serialport; // file handle for the serial port
char user_command_buffer[COMMANDBUFFERSIZE]; // holds characters typed by the user
char serial_command_buffer[COMMANDBUFFERSIZE]; // holds characters sent over the serial port
char serial_command_index; // index for the next empty position in the serial_command_buffer
char last_command_received[COMMANDBUFFERSIZE];
char last_command_sent[COMMANDBUFFERSIZE];
char number_of_bytes_sent;
int logfile; // file handle for the log file

#include "display.h" // requires access to some of the globals, so must be included after them

int main(int argc, char *argv[]){
	char* serial_port_name;
	if (argc ==1){
		serial_port_name = SERIALPORTDEFAULT;
	}else if (argc == 2){ // if a command line argument was supplied
		serial_port_name = argv[1];
	}else if(argc > 2){
		printf("Usage is: monitor [serial port name]\n");
	}
	PRINTDEBUG&&printf("main called..\n");
	setup_serial_port(serial_port_name);
	PRINTDEBUG&&printf("done setup...\n");
	
	setup_display();
	configure_user_input();
	
	// setup the log file
	if (LOG)
		logfile = open("log",O_WRONLY|O_APPEND|O_CREAT,0666);// the 0666 is an octal number which sets up proper permissions so we can actually open the log file after

	//blank out the buffers
	clear_user_command_buffer();
	clear_serial_command_buffer();
	while(1){
		read_from_serial_port();
		buffer_user_input();
		refresh_display();
	}
	return quit();
}

void clear_serial_command_buffer(){
	//blank out the serial buffer
	char i;
	for(i=0; i<COMMANDBUFFERSIZE;i++)
		serial_command_buffer[i] = '\0';
	serial_command_index = 0; // reset to the beginning of the buffer
}

void read_from_serial_port(){
	char buf;
	char i = read(serialport,&buf,1);	
	if (i == 1){
		serial_command_buffer[serial_command_index] = buf;
		serial_command_index++;
		serial_command_buffer[serial_command_index] = '\0';
		
		if ((serial_command_index>2)&&((serial_command_buffer[serial_command_index-1]=='\n')&&(serial_command_buffer[serial_command_index-2]=='\r'))){
			serial_command_buffer[serial_command_index-2] = '\0'; // strip of CR NL
			process_serial_command();
			clear_serial_command_buffer();
		}else if (serial_command_index >= COMMANDBUFFERSIZE-1){
		//	sprintf(serial_command_buffer,"Command Too Long.");
			log_serial_command();
			clear_serial_command_buffer();
		}
	}
}

void process_serial_command(){
	if (serial_command_buffer[0] == SENSORDATAPACKETCHARACTER){
		save_sensor_data_packet(serial_command_buffer[1]);
		if (LOGSPAM)
			log_serial_command();
	}else{
		sprintf(last_command_received,"%s",serial_command_buffer);
		log_serial_command();
	}
}

void save_sensor_data_packet(char code){
	long value;
	sscanf(serial_command_buffer + 2,"%ld",&value);
	switch(code){
		case COMPASSHEADING:
			sensor_data_cache.compass_heading = value;
			break;
		case SONARDISTANCE:	
			sensor_data_cache.filt_altitude = value;
			break;
		case BAROMETERTEMPERATURE:
			sensor_data_cache.barometer_temperature = value;
			break;
		case BAROMETERPRESSURE:
			sensor_data_cache.barometer_pressure = value;
			break;
		case BAROMETERALTITUDE:
			sensor_data_cache.barometer_altitude = value;
			break;
		case GYROSCOPEX:
			sensor_data_cache.gyroscope_x_rotational_velocity = value;
			break;
		case GYROSCOPEY:
			sensor_data_cache.gyroscope_y_rotational_velocity = value;
			break;
		case GYROSCOPETEMPERATURE:
			sensor_data_cache.gyroscope_temperature = value;
			break;
		case NUNCHUCKX:
			sensor_data_cache.nunchuck_x_value = value;
			break;
		case NUNCHUCKY:
			sensor_data_cache.nunchuck_y_value = value;
			break;
		case NUNCHUCKZ:
			sensor_data_cache.nunchuck_z_value = value;
			break;
		case YAW:
			sensor_data_cache.yaw = value;
			break;
		case PITCH:
			sensor_data_cache.filt_pitch_angle = value;
			break;
		case ROLL:
			sensor_data_cache.filt_roll_angle = value;
			break;
		case ALTGAIN:
			sensor_data_cache.alt_gain = value;
			break;
		case ROLLGAIN:
			sensor_data_cache.roll_gain = value;
			break;
		case PITCHGAIN:
			sensor_data_cache.pitch_gain = value;
			break;
		default:
			sprintf(serial_command_buffer,"Bad Sensor Data Packet.");
	}
}

void log_serial_command(){
	if (LOG){
		write(logfile,"SERIAL: ",8);
		write(logfile,serial_command_buffer,strlen(serial_command_buffer));
		write(logfile,"\n",1);
	}
}

void clear_user_command_buffer(){
	//blank out the input buffer
	char i;
	for(i=0; i<COMMANDBUFFERSIZE; i++)
		user_command_buffer[i] = '\0';
}

void transmit(char* c){
	number_of_bytes_sent = write(serialport,c,strlen(c));
	sprintf(last_command_sent,"%s",c);
	last_command_sent[strlen(last_command_sent)-2] = '\0';
	if (LOG){
		write(logfile,"PC: ",4);
		write(logfile,c,strlen(c)-2);
		write(logfile,"\n",1);
	}
}

void setup_serial_port(char* serial_port_name){	
	PRINTDEBUG&&printf("about to open port...\n");
	serialport = open(serial_port_name,O_NONBLOCK|O_RDWR|O_NOCTTY);
	PRINTDEBUG&&printf("opened port, now check if null\n");
	if (serialport  == -1){
		printf("Error: Unable to open serial port: %s\n",serial_port_name);	
		exit(1);
	}
	
	PRINTDEBUG&&printf("opened port successfully...\n");
	struct termios attribs;
	tcgetattr(serialport,&attribs);
	
	PRINTDEBUG&&printf("got attributes ok...\n");
	attribs.c_cflag &= ~CSIZE;
	attribs.c_cflag |= CS8;
	attribs.c_cflag |= CLOCAL|CREAD;
	attribs.c_cflag |= CSTOPB;
	attribs.c_iflag |= IGNPAR;
	attribs.c_oflag |= 0;
	attribs.c_lflag &= ~ICANON;
	cfmakeraw(&attribs);
	cfsetispeed(&attribs,PCBAUDRATE);
	cfsetospeed(&attribs,PCBAUDRATE);
	
	tcsetattr(serialport,TCSANOW,&attribs);
	PRINTDEBUG&&printf("done with attributes...\n");
}

// sets up  a custom 'raw' mode for handling user input
void configure_user_input(){
	cbreak(); // get characters types immediately
	noecho(); // don't print anything the user types
	nodelay(stdscr,TRUE);
}

int quit(){
	quit_display();
	close(serialport);
	if (LOG)
		close(logfile);
	exit(0);
	return 0;
}

void buffer_user_input(){
	char c[2];
	int character = getch();
	c[1] = '\0';
	if (character != ERR){
		if ((character == '\r')||(character == '\n')){
			process_user_input();
			clear_user_command_buffer();
		}else if (character == 0x7f){ // handle delete
			char bufferlength = strlen(user_command_buffer);
			if (bufferlength > 0){
				user_command_buffer[bufferlength-1] = '\0';
			}
		}else if(strlen(user_command_buffer)< COMMANDBUFFERSIZE-3){
			c[0] = character;
			strcat(user_command_buffer,c);
		}
	}
}

void process_user_input(){
	if ((!strcmp("q",user_command_buffer))||(!strcmp("Q",user_command_buffer))){
		quit();
	}else if(!strcmp("r",user_command_buffer)){
		transmit("RESET\r\n");
	}else{
		strcat(user_command_buffer,"\r\n");	
		transmit(user_command_buffer);
	}
}
