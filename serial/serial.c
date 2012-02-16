#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <unistd.h>
#include "charlie.c"

int main(void) {
	//set port B to input mode so we ignore those pins
	DDRB = 0;
	PORTB = 0;
	setup_serial();
	sei();	
	while(1){
	
	}
	return 0;
ISR(TWI_vect){
	if (){
		error();
	}
}
void transmit(char byte){
	
}
void setup_serial(){

}

// turns on the white LED for an error code
int error(){
	DDRB |= 2;
	PORTB &= ~2;
}
}
