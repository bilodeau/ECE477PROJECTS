#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ncurses.h>
#include <string.h>
#include "baud.h"

#define LOG 1// saves all commands into a log file
#define ENABLESERIAL 1
#define PRINTDEBUG 0 // prints debug info to the console when setting up the serial port
#define COMMANDBUFFERSIZE 200
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
void send_IR_code(char code_number);
void parse_codes();
void learn_command(char *description);

int serial_command_ready = 0;

char* IR_codes[10];
char* IR_code_descriptions[10];
int serialport; // file handle for the serial port
char user_command_buffer[COMMANDBUFFERSIZE]; // holds characters typed by the user
char serial_command_buffer[COMMANDBUFFERSIZE]; // holds characters sent in from the serial port
unsigned char serial_command_index; // points to the next empty position in the serial_command_buffer
int logfile; // file handle for the log file

int main(int argc, char *argv[]){
	char* serial_port_name;
	if (argc ==1){
		serial_port_name = SERIALPORTDEFAULT;
	}else if (argc == 2){ // if a command line argument was supplied
		serial_port_name = argv[1]; // the argument is the name of the serial port to open
	}else if(argc > 2){
		printf("Usage is: monitor [serial port name]\n");
	}
	PRINTDEBUG&&printf("main called..\n");
	if(ENABLESERIAL)mysetup_serial_port(serial_port_name);
	PRINTDEBUG&&printf("done setup...\n");
	configureUI();
	
	// parse the IR codes from file into the IR_codes array, and code descriptions in IR_code_descriptions array
	PRINTDEBUG&&printf("begin parsing codes...\n");
	parse_codes();
	PRINTDEBUG&&printf("done parsing codes...\n");

	// setup the log file
	if (LOG)
		logfile = open("log",O_WRONLY|O_APPEND|O_CREAT,0666);// the 0666 is an octal number which sets up proper permissions so we can actually open the log file after writing to it

	//blank out the buffers
	clear_user_command_buffer();
	clear_serial_command_buffer();
	while(1){
	if(ENABLESERIAL)read_from_serial_port(); // read in and buffer stuff in from the serial port
		if(serial_command_ready){
			post_serial_command();
			clear_serial_command_buffer();
			serial_command_ready = 0;
		}
		buffer_input();		 // buffer user input
		refresh();		 // refresh the display
	}
	return quit();
}

void learn_command(char* description){
	FILE* file = fopen("learnedcodes.txt","w+");
	if(file == NULL){
		printf("Error Opening or Creating learnedcodes.txt.\n");
		quit();
	}
	clear_serial_command_buffer();
	while(!serial_command_ready); // wait for the AVR to reply
	serial_command_ready = 0;
	sprintf(serial_command_buffer,"test");
	fwrite(serial_command_buffer,1,strlen(serial_command_buffer),file);
	fwrite(" ",1,1,file);
	fwrite(description,1,strlen(description),file);
	fwrite("\n",1,1,file);
	clear_serial_command_buffer();
}

void parse_codes(){
	FILE* file = fopen("codes.txt","r");
	if(file == NULL){
		printf("Error Opening codes.txt.\n");
		quit();
	}
	PRINTDEBUG&&printf("codes.txt opened OK\n");
	int i;
	for(i=0;i<10;i++){
		IR_codes[i] = malloc(200);
		IR_code_descriptions[i] = malloc(200);
		char *line= malloc(200);
		line[0] = '\0';
		int nbytes = 1; // the number of bytes we've allocated
		while(strlen(line) < 20){ // need at least 20 bytes to be a valid code, this will disregard any blank lines in the file 
			getline(&line,(size_t *)&nbytes,file); // getline automatically enlarges the allocated memory if needed
		}
		PRINTDEBUG&&printf("got line\n");
		//printf("line is: %s",line);
		sscanf(line,"%[0-9a-fA-F ]",IR_codes[i]); // relies on all descriptions starting with a non-hex character
		IR_codes[i][strlen(IR_codes[i])-2] = '\0'; // gets rid of any trailing space after the code
		sscanf(line + strlen(IR_codes[i])+1,"%[^\n\r]",IR_code_descriptions[i]);
		PRINTDEBUG&&printf("i: %d\n",i);
		PRINTDEBUG&&printf("%s",IR_codes[i]);
		PRINTDEBUG&&printf("\n");
		PRINTDEBUG&&printf("%s",IR_code_descriptions[i]);
		PRINTDEBUG&&printf("\n");
	// prints out the descriptions to the curses display
	move(i,0);
	printw("%d: ",i);
	printw("%s",IR_code_descriptions[i]);
	}
	fclose(file);
}

void send_IR_code(char code_number){
	transmit("\r\n");
	transmit(IR_codes[code_number]);
	transmit("\r\n");
}

void clear_serial_command_buffer(){
	//blank out the serial buffer
	unsigned char i;
	for(i=0; i<COMMANDBUFFERSIZE;i++)
		serial_command_buffer[i] = '\0';
	serial_command_index = 0; // reset to the beginning of the buffer
}

// check if there's any new bytes waiting on the serial port
void read_from_serial_port(){
	char buf;
	unsigned char i = read(serialport,&buf,1);	
	if (i == 1){ // if we read in a character, put it into the buffer and move down the null
		serial_command_buffer[serial_command_index] = buf;
		serial_command_index++;
		serial_command_buffer[serial_command_index] = '\0';
		// if the command is complete (ending with carriage return newline)
		if ((serial_command_index>2)&&((serial_command_buffer[serial_command_index-1]=='\n')&&(serial_command_buffer[serial_command_index-2]=='\r'))){
			serial_command_buffer[serial_command_index-2] = '\0'; // strip of CR NL
			serial_command_ready = 1;
		// we never got terminating characters so this command is too long
		}else if (serial_command_index >= COMMANDBUFFERSIZE-1){
			// overwrite the bad command with an error message
			sprintf(serial_command_buffer,"Command Too Long.");
			serial_command_ready = 1;
		}
	}
}

// prints the contents of the serial_command_buffer into a designated region of the screen
// uses the ncurses library, also prints commands out to the log file
void post_serial_command(){
	move(10,6);
	printw("                                        ");
	move(10,6);
	printw("%s",serial_command_buffer);
	move(11,6);
	printw("                                                            ");
	move(11,6);
	unsigned char i;
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
	unsigned char i;
	for(i=0; i<COMMANDBUFFERSIZE; i++)
		user_command_buffer[i] = '\0';
	move(15,0);
	printw(":                                                   ");
}

// sends the given string out over the serial port and prints it to the log file
// also prints some useful info the the console for debugging
void transmit(char* c){
	move(16,0);
	printw("                   ");
	int test = write(serialport,c,strlen(c));
	move(20,0);
	printw("               ");
	move(20,0);
	printw("bytes sent:  %d",test);
	move(22,0);
	printw("last string sent:");
	move(23,0);
	printw("                        ");
	move(23,0);
	printw(c);
	if (LOG){
		write(logfile,"PC: ",4);
		write(logfile,c,strlen(c)-2);
		write(logfile,"\n",1);
	}
}

// sets up the serial port using the baudrate given in baud.h
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
	attribs.c_cflag &= ~CSIZE;  // this masking is required when setting the frame size
	attribs.c_cflag |= CS8; // use 8 bit frames
	attribs.c_cflag |= CLOCAL|CREAD; // do not change owner of the port / enable receiver
	attribs.c_iflag |= IGNPAR; // don't use any parity checking
	attribs.c_oflag |= 0;
	attribs.c_lflag &= ~ICANON; // use raw mode (don't wait for carriage return or newline)
	cfmakeraw(&attribs);
	cfsetispeed(&attribs,PCBAUDRATE); // setup the baudrate based on values in baud.h
	cfsetospeed(&attribs,PCBAUDRATE);
	
	tcsetattr(serialport,TCSANOW,&attribs);  // configure the port to use these new settings
	PRINTDEBUG&&printf("done with attributes...\n");
}

// sets up curses and a custom 'raw' mode for handling user input and
// data display
void configureUI(){
	initscr();  // setup the curses screen
	cbreak(); // get characters types immediately
	noecho(); // don't print anything the user types
	nodelay(stdscr,TRUE);  // setup getch() to be non-blocking
	move(10,0);
	printw("AVR : "); // draw a label to the console
	move(14,0);
	printw("Type 'q' to quit"); // and a helpful message
}

int quit(){
	endwin(); // end curses window mode
	close(serialport);
	if (LOG)
		close(logfile);
	exit(0);
	return 0;
}

// buffers user input as it's typed so we don't have to block
void buffer_input(){
	char c[2];
	int character = getch();
	c[1] = '\0';
	if (character != ERR){ // getch() returns ERR if there wasn't any input
		move(25,0);	
		printw("character pressed:  %x",character);
		char code_number = character - '0';
		if ((code_number >= 0)&&(code_number <= 9)){ // if the user pressed a number key, we want to send it immediately
			send_IR_code(code_number);
			clear_user_command_buffer();
		}else if ((character == '\r')||(character == '\n')){
			process_input();
			clear_user_command_buffer();
		}else if (character == 0x7f){ // handle delete so we can backspace
			unsigned char bufferlength = strlen(user_command_buffer);
			if (bufferlength > 0){
				user_command_buffer[bufferlength-1] = '\0';
			}
		}else if(strlen(user_command_buffer)< COMMANDBUFFERSIZE-3){
			c[0] = character;
			strcat(user_command_buffer,c);
		}
	move(15,0);
	printw(":                                  ");
	move(15,0);
	printw(": %s",user_command_buffer); // print out the command buffer so that
					    // the user can see what they are typing
	}
}

// when the user has typed a complete command and pressed enter, send it to the AVR
void process_input(){
	if ((!strcmp("q",user_command_buffer))||(!strcmp("Q",user_command_buffer))){
		quit();  // quit this program
	}else if ((!strncmp("l",user_command_buffer,1))||(!strncmp("L",user_command_buffer,1))){
		transmit("l\n\r"); // put the AVR into learning mode
		learn_command(user_command_buffer + 2); // get ready to capture the response and add it to a file
	}else{
		strcat(user_command_buffer,"\r\n"); // AVR expects commands to be terminated
						    // by CR-NL
		transmit(user_command_buffer);  // now send the command to the AVR
	}
}
