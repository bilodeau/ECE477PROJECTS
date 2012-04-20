#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "baud.h"

#define PRINTDEBUG 0
#define COMMANDBUFFERSIZE 81
#define SERIALPORTDEFAULT "/dev/tty.usbmodemfd1221"

void clear_serial_command_buffer();
void setup_serial_port(char* serial_port_name);
void read_from_serial_port();

struct termios originalsettings;
int serialport; // file handle for the serial port
char serial_command_buffer[COMMANDBUFFERSIZE]; // holds characters sent over the serial port
char serial_command_index; // index for the next empty position in the serial_command_buffer
int logfile; // file handle for the log file

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
	
	// setup the log file
	logfile = open("temp_log",O_WRONLY|O_APPEND|O_CREAT,0666);// the 0666 is an octal number which sets up proper permissions so we can actually open the log file after
	write(serialport,"\r\n",2); // send a 'wake up' to the avr
	clear_serial_command_buffer();
	while(1){
		read_from_serial_port();
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
			write(logfile,serial_command_buffer,strlen(serial_command_buffer));
			PRINTDEBUG&&printf("logging temperature now..\n");
			clear_serial_command_buffer();
		}else if (serial_command_index >= COMMANDBUFFERSIZE-1){
			// command was too long to throw it out
			clear_serial_command_buffer();
		}
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

int quit(){
	close(serialport);
	close(logfile);
	exit(0);
	return 0;
}
