CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega328p -Wno-char-subscripts
OBJ2HEX=avr-objcopy
#UISP=/usr/local/bin/uisp
TARGET=test
ADFLAGS=-p atmega328p -c avrispmkII -P usb

.PHONY: fuses prog erase monitor status


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
	avrdude $(ADFLAGS) -U lfuse:w:0xe2:m #http://www.engbedded.com/cgi-bin/fc.cgi 
	avrdude $(ADFLAGS) -U hfuse:w:0xd9:m
	avrdude $(ADFLAGS) -U efuse:w:0xff:m

monitor:
	gcc PCmonitor.c -o monitor -lncurses

status:
	gcc PCstatus.c -o status -lncurses
