#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#define BAUDRATE 4800
#define FOSC 1843200
#define MYUBRR FOSC/16/BAUDRATE-1

void setup_serial();
void setup_PWM();
int error();
void transmit(char* byte);
void receive();
void process_command();

char receive_buffer[21];
char brightness = 0;

int main(void) {
	setup_serial();
	setup_PWM();

	while(1){
		receive();
		process_command();
		OCR1A = brightness*brightness;
	}
	return 0;
}

void process_command(){
	if (!strncmp(receive_buffer,"GET",3)){
		if ((!strncmp(receive_buffer+3," B1",3))&&(receive_buffer[6] == '\0')){
			char echo[20];
			sprintf(echo,"B1 %d\r\n",brightness);
			transmit(echo);
		}else{
			transmit("UNIMPLEMENTED\r\n");
		}
	}else if(!strncmp(receive_buffer,"SET",3)){
		if (!strncmp(receive_buffer+3," B1 ",4)){
			char temp;
			if ((sscanf(receive_buffer+7,"%d",temp)==1)&&((temp>=0)&&(temp<=100))){
				brightness = temp;
				char echo[20];
				sprintf(echo,"B1 %d\r\n",brightness);
				transmit(echo);
			}else{
				transmit("ILLEGAL VALUE\r\n");
			}
		}else{
			transmit("UNIMPLEMENTED\r\n");
		}
	}		
}

// the PC will be expecting "\r\n" as the last two bytes of this command
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
		receive_buffer[x] = UDR;
		if((receive_buffer[x] == '\n')&&(receive_buffer[x-1] == '\r')){
			receive_buffer[x+1] = '\0';
			break;
		}
	}
}

void setup_PWM(){
	DDRB = 2;
	OCR1A = 0;
	TCCR1A = 0xc0;
	TCCR1B = 0x11;
	ICR1 = 10000;
	sei();
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
