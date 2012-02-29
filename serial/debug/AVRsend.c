#include <avr/io.h>
#include "baud.h"

void setup_serial();
void transmit(char c);

int main(){
	setup_serial();

DDRB = 2;
PORTB = 0;
	
	while (1)
		transmit('a');	
	
	return 0;
}

void setup_serial(){
	// use baud rate and frame settings from baud.h
	UBRRH = MYUBRRH;
	UBRRL = MYUBRRL;
	UCSRA = MYUCSRA;
	UCSRC = MYUCSRC;
	
	// enable transmitter
	UCSRB |= (1<<TXEN);
}

void transmit(char c){
	while(!(UCSRA & (1<<UDRE)));
		UDR = c;
}
