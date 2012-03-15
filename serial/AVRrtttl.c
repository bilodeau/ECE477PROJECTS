#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "baud.h"

void setup_serial();
void setup_PWM();
void transmit(char* byte);
void receive();
void process_command();

char receive_buffer[21];
char brightness = 0;

int main(void) {
	setup_serial();
	setup_PWM();

	while(1){
		receive(); // block waiting for the next command from the PC
		process_command();  // we've gotten a command, so process it
		OCR1A = brightness*brightness; // set the brightness of the LED accordingly
	}
	return 0;
}

// processes the command we've already put into the receive buffer
void process_command(){
	if (!strncmp(receive_buffer,"GET",3)){ // if command starts with GET
		if ((!strncmp(receive_buffer+3," B1",3))&&(receive_buffer[6] == '\0')){
			char echo[20];
			sprintf(echo,"B1 %d\r\n",brightness);
			transmit(echo); // transmit the current brightness back to the PC
		}
	}else if(!strncmp(receive_buffer,"SET",3)){ // command starts with SET
		if (!strncmp(receive_buffer+3," B1 ",4)){
			int temp;
			if ((sscanf(receive_buffer+7,"%d",&temp)==1)&&((temp>=0)&&(temp<=100))){
				brightness = temp; // set the brightness to the value given
				char echo[20];
				sprintf(echo,"B1 %d\r\n",brightness);
				transmit(echo); // echo the brightness back to PC
			}else{
				transmit("ILLEGAL VALUE\r\n"); // "SET" was ok, but we had a bad integer provided
			}
		}
	}else{		
		transmit("UNIMPLEMENTED\r\n"); // we received a command other than GET or SET
	}
}

// sends the specified, null-terminated string to the PC
// the PC will be expecting "\r\n" as the last two bytes of this command
void transmit(char* str){
	char x;
	for(x=0; x <strlen(str);x++){
		while( !(UCSRA & (1<<UDRE))); // wait for the data register to be empty
		UDR = str[x]; // transmit the next byte
	}
}

// receives a serial command from the PC
void receive(){
	receive_buffer[20] = '\0';
	int x;
	for(x=0;x<20;x++){
		while (!(UCSRA & (1<<RXC))); // wait for the next byte
		receive_buffer[x] = UDR;  // received a byte so copy it into the buffer

		// if we've received terminating characters
		if((receive_buffer[x] == '\n')&&(receive_buffer[x-1] == '\r')){
			receive_buffer[x+1] = '\0'; // add a null character
			break;
		}
	}
}

// sets up pulse width modulation fro controlling led brightness
void setup_PWM(){
	DDRB = 2;  // use PB1 as an output pin
	OCR1A = 0;  // initial compare value is zero (zero duty cycle = zero brightness)
	TCCR1A = 0xc0; // set on compare match (the led is connected high so this turns it off) 
	TCCR1B = 0x11; // WGM mode 8, prescaler 1
	ICR1 = 10000; // TOP value
	sei();
}

void setup_serial(){
	// setup baud rate and frame according to the defines in baud.h
	UBRRH = MYUBRRH;
	UBRRL = MYUBRRL;
	UCSRA = MYUCSRA;
	UCSRC = MYUCSRC;

	// enable receiver and transmitter
	UCSRB = (1<<RXEN)|(1<<TXEN);
	
	// configure interrupts
	UCSRB |= (0<<RXCIE)|(0<<TXCIE)|(0<<UDRIE);	
}
