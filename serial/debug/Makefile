CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega8
OBJ2HEX=avr-objcopy
#UISP=/usr/local/bin/uisp
TARGET=AVRsend
SEND=AVRsend
RECEIVE=AVRreceive
ADFLAGS=-p m8 -c avrispmkII -P usb

.PHONY: fuses prog erase pc receive send


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
	 avrdude $(ADFLAGS) -e
clean :
	rm -f *.hex *.obj *.o *.eeprom

# 8Mhz clock speed
fuses:
	avrdude $(ADFLAGS) -U lfuse:w:0xE4:m #http://www.engbedded.com/cgi-bin/fc.cgi 
	avrdude $(ADFLAGS) -U hfuse:w:0xd9:m

pc:
	gcc PCsend.c -o pcs
	gcc PCreceive.c -o pcr

send: $(SEND).hex $(SEND).eeprom
	avrdude $(ADFLAGS) -V -U flash:w:$(TARGET).hex:i

receive: $(RECEIVE).hex $(RECEIVE).eeprom
	avrdude $(ADFLAGS) -V -U flash:w:$(TARGET).hex:i
