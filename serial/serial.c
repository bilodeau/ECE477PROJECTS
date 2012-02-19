#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "charlie.c"

#define BAUDRATE 4800
#define FOSC 1843200
#define MYUBRR FOSC/16/BAUDRATE-1
#define UPDATEPERIOD  4000 // 40k is about 3 sec

void setup_serial();
void setup_timer();
int error();
void transmit(char* byte);
void receive();

char receive_buffer[21];

int main(void) {
	setup_serial();
//	setup_timer();
	sei();// enable global interrupts

	while(1){
	// echos recieved characters back, but with brackets
		receive();
		char temp[23];
		sprintf(temp,"[%s]",receive_buffer);
		transmit(temp);
	}
	return 0;
}

void transmit(char* str){
	char x;
	for(x=0; x <strlen(str);x++){
		while( !(UCSRA & (1<<UDRE)));
		UDR = str[x];
	}
}

void receive(){
	receive_buffer[20] = '\0';
	int x;
	for(x=0;x<20;x++){
		while (!(UCSRA & (1<<RXC)));
		if((receive_buffer[x] = UDR) == '\0'){
		error();
		 break;
		}
	}
}


void setup_timer(){
	TCCR1A = 0;
	TCCR1B = 0x0b;
	OCR1A = UPDATEPERIOD;
	TIMSK = 16;
}

ISR(TIMER1_COMPA_vect){
}

ISR(USART_TXC_vect){
}

ISR(USART_RXC_vect){
}

ISR(USART_UDRE_vect){
}

void setup_serial(){
	// setup baud rate
	UBRRH = 0;//(unsigned char) ((MYUBRR)>>8);
	UBRRL = 12;//(unsigned char) MYUBRR;
	
	UCSRA |= (1<<UDRE)|(1<<U2X); // turn off double speed mode!!
	// enable receiver and transmitter
	UCSRB = (1<<RXEN)|(1<<TXEN);
	// set frame format: 8 data, 2 stop bit
	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
	
	// configure interrupts
	UCSRB |= (0<<RXCIE)|(0<<TXCIE)|(0<<UDRIE);	
}

// turns on the white LED for an error code
int error(){
	DDRB |= 2;
	PORTB &= ~2;
	return 0;
}
