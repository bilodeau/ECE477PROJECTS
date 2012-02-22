#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ncurses.h>
#include <string.h>
#include "baud.h"

#define PRINTDEBUG 1
#define USERCOMMANDBUFFERSIZE 23
#define SERIALPORT "/dev/tty.usbmodemfd1231"

void clear_user_command_buffer();
void mysetup_serial_port();
void buffer_input();
void process_input();
void configureUI();
void run_in_loop();
void transmit(char* c);

struct termios originalsettings;
int serialport;
char user_command_buffer[USERCOMMANDBUFFERSIZE];

int main(){
	PRINTDEBUG&&printf("main called..\n");
	mysetup_serial_port();
	PRINTDEBUG&&printf("done setup...\n");
	configureUI();

	//blank out the input buffer
	char i;
	for(i=0; i<43; i++)
		user_command_buffer[i] = '\0';
	char buf[21];
	buf[0] = '\0';
	while(1){
		char i = read(serialport,buf,USERCOMMANDBUFFERSIZE-1);	
		if (i < 3){
			// No data read, or we only got "\r\n".
		}else{
			buf[i-2] = '\0';
			move(0,6);
			printw("                    ");
			move(0,6);
			printw("%s",buf);
			move(1,6);
			printw("                    ");
			move(1,6);
			printw("%x",buf);
		}
		run_in_loop();
	}
	return quit();
}
void clear_user_command_buffer(){
	//blank out the input buffer
	char i;
	for(i=0; i<USERCOMMANDBUFFERSIZE; i++)
		user_command_buffer[i] = '\0';
	move(5,0);
	printw("                              ");
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
	char temp[strlen(c)-1];
	snprintf(temp,strlen(c)-2,"%s",c);
	printw(c);
}

void mysetup_serial_port(){	
	PRINTDEBUG&&printf("about to open port...\n");
	serialport = open(SERIALPORT,O_NONBLOCK|O_RDWR|O_NOCTTY);
	PRINTDEBUG&&printf("opened port, now check if null\n");
	if (serialport  == -1){
		printf("Error: Unable to open serial port.\n");	
		exit(1);
	}
	
	PRINTDEBUG&&printf("opened port successfully...\n");
	struct termios attribs;
	tcgetattr(serialport,&attribs);
	
	PRINTDEBUG&&printf("got attributes ok...\n");
	attribs.c_cflag &= ~CSIZE;
	attribs.c_cflag |= CS8;
	attribs.c_cflag |= CLOCAL|CREAD;
	attribs.c_iflag |= IGNPAR;
	attribs.c_oflag |= 0;
	attribs.c_lflag &= ~ICANON;
	cfmakeraw(&attribs);
	cfsetispeed(&attribs,BAUDRATE);
	cfsetospeed(&attribs,BAUDRATE);
	
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

void run_in_loop(){
	buffer_input();
	refresh(); // refreshes curses window
}

int quit(){
	endwin(); // end curses window mode
	close(serialport);
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
		}else if (character == 0x7f){ // handle backspace
			char bufferlength = strlen(user_command_buffer);
			if (bufferlength > 0){
				user_command_buffer[bufferlength-1] = '\0';
			}
		}else if(strlen(user_command_buffer)< USERCOMMANDBUFFERSIZE-3){
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
		exit(quit());
	}else{
		strcat(user_command_buffer,"\r\n");	
		transmit(user_command_buffer);
	}
}
