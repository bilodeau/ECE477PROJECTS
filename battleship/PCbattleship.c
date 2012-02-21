#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ncurses.h>
#include <string.h>

#define BAUDRATE B9600
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
		printw("                    ");
		move(0,6);
		printw("%s",buf);
		run_in_loop();
	}
	return quit();
}

void transmit(char* c){
	if (strlen(c)>20){
		move(6,0);
		printw("Command Too Long...");
		move(10,0);
		printw("                                ");
	}else{
		move(6,0);
		printw("                   ");
		int test = write(serialport,&c,strlen(c));
		move(10,0);
		printw("               ");
		move(10,0);
		printw("bytes sent:  %d",test);
		move(12,0);
		printw("last string sENT NOT DONE");
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
	//cbreak(); // get characters types immediately
	//noecho(); // don't print anything the user types
	//nodelay(stdscr,TRUE);
	move(1,0);
	printw("Type 'q' to quit");
}

void run_in_loop(){
	refresh();
	process_input();
	refresh(); // refreshes curses window
}

int quit(){
	endwin(); // end curses window mode
	close(serialport);
	return 0;
}

void process_input(){
	move(2,0);
	printw(": ");
	char c = getch();
	if (c != ERR){
		char buf[21];
		buf[20] = '\0';
		char i = 0;
		while((i < 20)&&(c != ERR)&&(c != '\n')&&(c != '\r')){
			buf[i] = c;
			c = getch();
			i++;
		}
		buf[i] = '\0';
		move(2,0);
		printw("                                          ");
		if ((!strcmp("q",buf))||(!strcmp("Q",buf))){
			exit(quit());
		}else{
			char temp[23];
			temp[0] = '\0';
			strcat(temp,buf);
			strcat(temp,"\r\n");	
			transmit(temp);
		}	
	}
}
