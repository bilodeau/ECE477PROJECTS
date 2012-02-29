#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ncurses.h>
#include <string.h>
#include "baud.h"

#define LOG 1// saves all commands into a log file
#define PRINTDEBUG 1
#define COMMANDBUFFERSIZE 120
#define SERIALPORTDEFAULT "/dev/tty.usbmodemfd1221"

void clear_serial_command_buffer();
void clear_user_command_buffer();
void mysetup_serial_port(char* serial_port_name);
void buffer_input();
void process_input();
void configureUI();
void transmit(char* c);
void read_from_serial_port();
void post_serial_command();

struct termios originalsettings;
int serialport;
char user_command_buffer[COMMANDBUFFERSIZE];
char serial_command_buffer[COMMANDBUFFERSIZE];
char serial_command_index;

int logfile;

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
	mysetup_serial_port(serial_port_name);
	PRINTDEBUG&&printf("done setup...\n");
	configureUI();
	
	// setup the log file
	if (LOG)
		logfile = open("log",O_WRONLY|O_APPEND|O_CREAT,0666);// the 0666 is an octal number which sets up proper permissions so we can actually open the log file after

	//blank out the buffers
	clear_user_command_buffer();
	clear_serial_command_buffer();
	while(1){
		read_from_serial_port();
		buffer_input();
		refresh();
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
			serial_command_buffer[serial_command_index-2] = '\0'; // strip off CR NL
			post_serial_command();
			clear_serial_command_buffer();
		}else if (serial_command_index >= COMMANDBUFFERSIZE-1){
			sprintf(serial_command_buffer,"Command Too Long.");
			post_serial_command();
			clear_serial_command_buffer();
		}
	}
}

void post_serial_command(){
	move(0,6);
	printw("                                        ");
	move(0,6);
	printw("%s",serial_command_buffer);
	move(1,6);
	printw("                                                            ");
	move(1,6);
	char i;
	for (i=0; i<serial_command_index-1;i++)
		printw("%x",serial_command_buffer[i]);

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
	move(5,0);
	printw(":                                                   ");
}

void transmit(char* c){
	move(6,0);
	printw("                   ");
	int test = write(serialport,c,strlen(c));
	move(10,0);
	printw("               ");
	move(10,0);
	printw("bytes sent:  %d",test);
	move(12,0);
	printw("last string sent:");
	move(13,0);
	printw("                        ");
	move(13,0);
	printw(c);
	if (LOG){
		write(logfile,"PC: ",4);
		write(logfile,c,strlen(c)-2);
		write(logfile,"\n",1);
	}
}

void mysetup_serial_port(char* serial_port_name){	
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
	attribs.c_cflag &= ~CSTOPB;
	attribs.c_cflag |= CLOCAL|CREAD;
	attribs.c_iflag |= IGNPAR;
	attribs.c_oflag |= 0;
	attribs.c_lflag &= ~ICANON;
	cfmakeraw(&attribs);
	cfsetispeed(&attribs,PCBAUDRATE);
	cfsetospeed(&attribs,PCBAUDRATE);
	
	tcsetattr(serialport,TCSANOW,&attribs);
	PRINTDEBUG&&printf("done with attributes...\n");
}

// sets up curses and a custom 'raw' mode for handling user input and
// data display
void configureUI(){
	initscr();  // setup the curses screen
	cbreak(); // get characters types immediately
	noecho(); // don't print anything the user types
	nodelay(stdscr,TRUE);
	move(0,0);
	printw("AVR : ");
	move(4,0);
	printw("Type 'q' to quit");
}

int quit(){
	endwin(); // end curses window mode
	close(serialport);
	if (LOG)
		close(logfile);
	exit(0);
	return 0;
}

void buffer_input(){
	char c[2];
	int character = getch();
	c[1] = '\0';
	if (character != ERR){
		move(15,0);	
		printw("character pressed:  %x",character);
		if ((character == '\r')||(character == '\n')){
			process_input();
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
	move(5,0);
	printw(":                                  ");
	move(5,0);
	printw(": %s",user_command_buffer);
	}
}

void process_input(){
	if ((!strcmp("q",user_command_buffer))||(!strcmp("Q",user_command_buffer))){
		quit();
	}else if(!strcmp("r",user_command_buffer)){
		transmit("RESET\r\n");
	}else{
		strcat(user_command_buffer,"\r\n");	
		transmit(user_command_buffer);
	}
}
