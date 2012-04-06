CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega8 -Wno-char-subscripts
OBJ2HEX=avr-objcopy
#UISP=/usr/local/bin/uisp
TARGET=AVRircapture
ADFLAGS=-p m8 -c avrispmkII -P usb

.PHONY: fuses prog erase pc


prog : $(TARGET).hex $(TARGET).eeprom
	avrdude $(ADFLAGS) -V -U flash:w:$(TARGET).hex:i
#	avrdude $(ADFLAGS) -U eeprom:w:$(TARGET).eeprom:i

%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

%.eeprom : %.obj
	$(OBJ2HEX) -j .eeprop -O ihex $< $@

erase :
	 avrdude $(ADFLAGS) -E noreset -e
clean :
	rm -f *.hex *.obj *.o *.eeprom

fuses:
	avrdude $(ADFLAGS) -U lfuse:w:0xE4:m #http://www.engbedded.com/cgi-bin/fc.cgi 
	avrdude $(ADFLAGS) -U hfuse:w:0xd9:m

pc:
	gcc PCir.c -o pc -lncurses
