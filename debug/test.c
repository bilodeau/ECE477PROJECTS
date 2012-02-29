#include <avr/io.h>
#include "baud.h"
#include <string.h>

void setup_serial();
void transmit(char* str);

int main (){

setup_serial();

while(1){
	DDRB= 2;
	PORTB = 0;
	transmit("Hello World!");
	PORTB = 2;
}

return 0;
}

void transmit(char* str){
	char x;
	for(x=0;x<strlen(str);x++){
		while (!(UCSR0A & (1<<UDRE0)));
		UDR0 = str[x];
	}
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = '\r';
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = '\n';
}

void setup_serial(){
        // setup baud rate and frame based on defines in baud.h
        UBRR0H = MYUBRRH;
        UBRR0L = MYUBRRL;
        UCSR0A = MYUCSRA;
        UCSR0C = MYUCSRC;
    
        // enable receiver and transmitter
        UCSR0B = (0<<RXEN0)|(1<<TXEN0);
}

