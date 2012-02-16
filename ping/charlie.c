#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>

int loop_counter = 0;
int int_counter = 0;
int mode = 0;
	// store 'on' flags in the low four bits of each char
	// each char represents a single row starting at the bottom
	// for example: displaying an 'X' 
	//	XXXX 1001  //flags[4]
	//	XXXX 1001  //flags[3]
	//	XXXX 0110  //flags[2]
	//	XXXX 1001  //flags[1]
	//	XXXX 1001  //flags[0]
char flags[5];

void led_on_i(int index);
void led_on(char x, char y);
void led_off();
void flash_leds();
void clear();
void lightrow(char row);
void lightcolumn(char col);
void lightled(char index);
void rowcolpattern(int counter);
void byindex(int index);

void lightled(char index){
	flags[index/4] |= 1<<(index%4);
}
/*
int main(void) {
	TCCR1A = 0;
	TCCR1B = 0x1b;
	ICR1 = 40000;
	OCR1A = 1500; 
	OCR1B = 40000;
	TIMSK = 24;
	sei();
	clear();
	while(1){
		flash_leds();
	}
	return 0;
}

ISR(TIMER1_COMPB_vect){
	mode++;	
	int_counter = 0;
}

ISR(TIMER1_COMPA_vect){
	OCR1A += 1500;
	if (OCR1A > 40000)
		OCR1A = 1500;
	int_counter++;
	clear();
	mode = mode % 4;
	if (mode == 0){
		if (int_counter > 4)
			int_counter = 0;
		lightcolumn(int_counter);
	}else if (mode == 1){
		 if (int_counter > 5)
			int_counter = 0;
		lightrow(int_counter);
	}else if (mode == 2){
		if (int_counter >4)
			int_counter = 0;
		rowcolpattern(int_counter);
	}else if (mode ==3){
		if (int_counter > 19)
			int_counter = 0;
		byindex(int_counter);
	}
}*/
	
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
void flash_leds(){
		char i;
		char j;
		for(i=0;i<5;i++){//row
			for(j=0;j<4;j++){// column
				if(((flags[i]>>j)&1) == 1){
					led_on(i,j);
				}else{
	//				led_off();
				}
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
	int highpin;
	int lowpin;
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
