// This header file contains baud rate settings for both the AVR and the PC.
// Thus, baud settings only need to be changed in this one place.

#define MYUBRRH 0
#define MYUBRRL 103 // UBRRH/L values from the table in the spec sheet
#define MYUCSRA (1<<UDRE)|(1<<U2X)
#define MYUCSRC (1<<URSEL)|(1<<USBS)|(3<<UCSZ0)
#define PCBAUDRATE B9600
