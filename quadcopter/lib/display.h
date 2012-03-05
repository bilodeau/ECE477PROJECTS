#ifndef DISPLAY
#define DISPLAY 1
#include <ncurses.h>

// sets up curses for data display
void setup_display(){
	initscr();
}

void refresh_display(){
	move(0,0);
	printw("                                        ");
	move(0,0);
	printw("Last Command Received: %s",last_command_received);
	char i;
	move(0,40);
	printw("Hex: ");
	for(i=0; i<strlen(last_command_received); i++)
		printw("%x",last_command_received[i]);

	move(2,0);
	printw("                                        ");
	move(2,0);
	printw("Compass Heading (degrees): %ld",sensor_data_cache.compass_heading);
	
	move(2,40);
	printw("                                        ");
	move(2,40);
	printw("Sonar Distance (feet): %ld",sensor_data_cache.sonar_distance);
	
	move(3,0);
	printw("                                        ");
	move(3,0);
	printw("Barometer Temperature: %ld",sensor_data_cache.barometer_temperature);

	move(3,40);
	printw("                                        ");
	move(3,40);
	printw("Barometer Pressure: %ld",sensor_data_cache.barometer_pressure);
	
	move(4,0);
	printw("                                        ");
	move(4,0);
	printw("Barometer Altitude: %ld",sensor_data_cache.barometer_altitude);
	
	move(5,0);
	printw("                                        ");
	move(5,0);
	printw("");
	
	move(20,0);
	printw("Type 'q' to quit.");
	
	move(21,0);
	printw("                                        ");
	move(21,0);
	printw(": %s",user_command_buffer);

	move(22,0);
	printw("                                        ");
	move(22,0);
	printw("Number of bytes sent:  %d",number_of_bytes_sent);

	move(23,0);
	printw("                                        ");
	move(23,0);
	printw("Last Command Sent: %s",last_command_sent);
	move(23,40);
	printw("Hex: ");
	for(i=0; i<strlen(last_command_sent); i++)
		printw("%x",last_command_sent[i]);
	
	// return the cursor to the command input line
	move(21,strlen(user_command_buffer)+2);
	refresh();
}

//ends the curses window mode
void quit_display(){
	endwin();
}

#endif
