#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ncurses.h>

#define BAUDRATE B4800
#define PRINTDEBUG 1

int serialport;
void mysetup_serial_port();
void process_input();
void configureUI();
void run_in_loop();
void transmit(char* c);

struct termios originalsettings;

int main(){
	PRINTDEBUG&&printf("main called..\n");
	mysetup_serial_port();
	PRINTDEBUG&&printf("done setup...\n");
	configureUI();
	char buf[21];
	buf[20] = '\0';
	while(1){
		move(0,0);
		printw("DATA: ");
		char i = read(serialport,buf,20);	
		if (i == -1){
			// No data read.
		}else{
			buf[i] = '\0';
		}
		printw("%s",buf);
		run_in_loop();
	}
	return quit();
}

void transmit(char* c){
	if (strlen(c)>20){
		move(6,0);
		printw("Command Too Long...");
	}else{
		int test = write(serialport,&c,strlen(c));
		move(10,0);
		printw("bytes sent:  %d",test);
	}
}

void mysetup_serial_port(){	
	PRINTDEBUG&&printf("about to open port...\n");
	serialport = open("/dev/tty.usbmodemfd1211",O_NONBLOCK|O_RDWR|O_NOCTTY);
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
//	cbreak(); // get characters types immediately
	noecho(); // don't print anything the user types
	nodelay(stdscr,TRUE);
	move(1,0);
	printw("Type 'q' to quit");
}

void run_in_loop(){
	process_input();
	refresh(); // refreshes curses window
}
int quit(){
	endwin(); // end curses window mode
	close(serialport);
	return 0;
}

void process_input(){
	
	int c = getch();
	if (c != ERR){
		move(2,0);
		printw("                                          ");
		switch(c){
			case 'q':
				exit(quit());break;
			default:
			//	move(2,0);
			//	printw("Command not recognized.");
				transmit(c);
		}	
		move(2,0);
		printw(": %c",c);
	}
}
