#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

	// store 'on' flags in the low four bits of each char
	// each char represents a single row starting at the bottom
	// for example: displaying an 'X' 
	//	XXXX 1001  //flags[4]
	//	XXXX 1001  //flags[3]
	//	XXXX 0110  //flags[2]
	//	XXXX 1001  //flags[1]
	//	XXXX 1001  //flags[0]
char flags[5];
char animation_flags[20];
char animation_counter = 0;
void led_on(char row, char col);
void led_off();
void led_on_i(int index);
void flash_leds();
void clear();
void lightrow(char row);
void lightcolumn(char col);
void lightled(char index);
void rowcolpattern(int counter);
void byindex(int index);


void setup_animation(){
	char i;
	for(i=0;i<20;i++)
		animation_flags[i] = 0;
}
void explosion_animation_loop(){
	animation_counter++;
	animation_counter %= 4;
	setup_animation(); // clears the board
	switch(animation_counter){
		case 0:	
			animation_flags[9] = 1;		
			animation_flags[10] = 1;
			break;
		case 1:	
			animation_flags[9] = 1;		
			animation_flags[10] = 1;
			animation_flags[5] = 1;
			animation_flags[6] = 1;
			animation_flags[14] = 1;
			animation_flags[13] = 1;
			break;
		case 2:	
			animation_flags[0] = 1;
			animation_flags[1] = 1;
			animation_flags[2] = 1;
			animation_flags[3] = 1;
			animation_flags[4] = 1;
			animation_flags[7] = 1;
			animation_flags[8] = 1;
			animation_flags[11] = 1;
			animation_flags[12] = 1;
			animation_flags[15] = 1;
			animation_flags[16] = 1;
			animation_flags[19] = 1;
			animation_flags[17] = 1;
			animation_flags[18] = 1;
			break;
		case 3:	
			break; // blank frame
	}
}
void radar_animation_loop(){
	animation_counter++;
	animation_counter %= 14;

	switch(animation_counter){
		case 0:
			// turn off previous flags
			animation_flags[5] = 0;
			animation_flags[1] = 0;
			// turn on next flags
			animation_flags[5] = 1;
			animation_flags[0] = 1;
			break;
		case 1:
			// turn off previous flags
			animation_flags[5] = 0;
			animation_flags[0] = 0;
			// turn on next flags
			animation_flags[5] = 1;
			animation_flags[4] = 1;
			break;
		case 2:
			// turn off previous flags
			animation_flags[5] = 0;
			animation_flags[4] = 0;
			// turn on next flags
			animation_flags[9] = 1;
			animation_flags[8] = 1;
			break;
		case 3:
			// turn off previous flags
			animation_flags[9] = 0;
			animation_flags[8] = 0;
			// turn on next flags
			animation_flags[13] = 1;
			animation_flags[12] = 1;
			break;
		case 4:
			// turn off previous flags
			animation_flags[13] = 0;
			animation_flags[12] = 0;
			// turn on next flags
			animation_flags[13] = 1;
			animation_flags[16] = 1;
			break;
		case 5:
			// turn off previous flags
			animation_flags[13] = 0;
			animation_flags[16] = 0;
			// turn on next flags
			animation_flags[13] = 1;
			animation_flags[17] = 1;
			break;
		case 6:
			// turn off previous flags
			animation_flags[13] = 0;
			animation_flags[17] = 0;
			// turn on next flags
			animation_flags[14] = 1;
			animation_flags[18] = 1;
			break;
		case 7:
			// turn off previous flags
			animation_flags[14] = 0;
			animation_flags[18] = 0;
			// turn on next flags
			animation_flags[14] = 1;
			animation_flags[19] = 1;
			break;
		case 8:
			// turn off previous flags
			animation_flags[14] = 0;
			animation_flags[19] = 0;
			// turn on next flags
			animation_flags[14] = 1;
			animation_flags[15] = 1;
			break;
		case 9:
			// turn off previous flags
			animation_flags[14] = 0;
			animation_flags[15] = 0;
			// turn on next flags
			animation_flags[10] = 1;
			animation_flags[11] = 1;
			break;
		case 10:
			// turn off previous flags
			animation_flags[10] = 0;
			animation_flags[11] = 0;
			// turn on next flags
			animation_flags[6] = 1;
			animation_flags[7] = 1;
			break;
		case 11:
			// turn off previous flags
			animation_flags[6] = 0;
			animation_flags[7] = 0;
			// turn on next flags
			animation_flags[6] = 1;
			animation_flags[3] = 1;
			break;
		case 12:
			// turn off previous flags
			animation_flags[6] = 0;
			animation_flags[3] = 0;
			// turn on next flags
			animation_flags[6] = 1;
			animation_flags[2] = 1;
			break;
		case 13:
			// turn off previous flags
			animation_flags[6] = 0;
			animation_flags[2] = 0;
			// turn on next flags
			animation_flags[5] = 1;
			animation_flags[1] = 1;
			break;
		}
	// these two leds will be 'constant on'
	animation_flags[9] = 1;
	animation_flags[10] = 1;
}
void lightled(char index){
	flags[index/4] |= 1<<(index%4);
}
	
void rowcolpattern(int counter){
	lightrow(counter);
	lightcolumn(counter);	
}

void lightrow(char row){
	flags[row] = 1+2+4+8; // turns on the led's for the specified row
}

void lightcolumn(char column){
	char j;
	for(j = 0; j<5;j++)
		flags[j] = (1<<column)|flags[j];
}
void clear(){
	char i;
	for(i=0; i<5;i++)
		flags[i] = 0;
}

// requires as a parameter a pointer to an 20-byte long array
// each byte should be either 0 or 1, representing either an off or on state
void flash_leds_from_array(char* array){
	char i;
	for(i=0; i <20; i++){
		if (array[i]){
			led_on_i(i);
			volatile char j;
			for(j=0;j<100;j++);
			led_off();
		}
	}
}

// requires as a parameter a pointer to an 20-byte long array
// each byte should be either 0 or 1, representing either an off or on state
void flash_leds_from_array_AND_NOT(char* array1, char* array2){
	char i;
	for(i=0; i <20; i++){
		if (array1[i] && (!array2[i])){
			led_on_i(i);
			volatile char j;
			for(j=0;j<100;j++);
			led_off();
		}
	}
}
void flash_leds(){
		char i;
		char j;
		for(i=0;i<5;i++){//row
			for(j=0;j<4;j++){// column
			//	led_off();
				if(((flags[i]>>j)&1) == 1)
					led_on(i,j);
			}
		}
}

// lights one led at a time
void byindex(int counter){
	led_on_i(counter);
}

// helper function that computes an led's index based on row/col
void led_on(char row,char col){
	led_on_i(col+4*row);
}

//called when we're not illuminating any LED and we want to shut everything off
void led_off(){
	DDRC = 0; // set all pins to input mode
	PORTC = 0;  //turn off all pullup resistors
}

// picks the right pins to use for charlieplexing
// this is specific to our exact wiring and may not match the circuit
// shown in the schematic
void led_on_i(int index){
	int highpin = 0;
	int lowpin = 0;
	switch(index){
		case 0:
			highpin = 4;
			lowpin = 3;
			break;
		case 1:
			highpin = 3;
			lowpin = 2;
			break;
		case 2:
			highpin = 2;
			lowpin = 1;
			break;
		case 3:
			highpin = 1;
			lowpin = 0;
			break;
		case 4:
			highpin = 3;
			lowpin = 4;
			break;
		case 5:
			highpin = 2;
			lowpin = 3;
			break;
		case 6:
			highpin = 1;
			lowpin = 2;
			break;
		case 7:
			highpin = 0;
			lowpin = 1;
			break;
		case 8:
			highpin = 4;
			lowpin = 2;
			break;
		case 9:
			highpin = 2;
			lowpin = 4;
			break;
		case 10:
			highpin = 0;
			lowpin = 2;
			break;
		case 11:
			highpin = 2;
			lowpin = 0;
			break;
		case 12:
			highpin = 1;
			lowpin = 4;
			break;
		case 13:
			highpin = 4;
			lowpin = 1;
			break;
		case 14:
			highpin = 3;
			lowpin = 0;
			break;
		case 15:
			highpin = 0;
			lowpin = 3;
			break;
		case 16:
			highpin = 3;
			lowpin = 1;
			break;
		case 17:
			highpin = 1;
			lowpin = 3;
			break;
		case 18:
			highpin = 4;
			lowpin = 0;
			break;
		case 19:
			highpin = 0;
			lowpin = 4;
			break;
	}
	DDRC = 0;
	PORTC = 0;
	DDRC = (1<<highpin)|(1<<lowpin);
	PORTC = (1<<highpin);
}
