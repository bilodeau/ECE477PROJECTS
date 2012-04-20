CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega8 -Wno-char-subscripts
OBJ2HEX=avr-objcopy
#UISP=/usr/local/bin/uisp
TARGET=AVRadc
TEST=test
ADFLAGS=-p m8 -c avrispmkII -P usb

.PHONY: fuses prog erase pc plot test


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

fuses:
	avrdude $(ADFLAGS) -U lfuse:w:0xE4:m #http://www.engbedded.com/cgi-bin/fc.cgi 
	avrdude $(ADFLAGS) -U hfuse:w:0xd9:m

test: $(TEST).hex $(TEST).eeprom
	avrdude $(ADFLAGS) -V -U flash:w:$(TEST).hex:i
	
pc:
	gcc PClogger.c -o pc

plot:
	gcc makeplot.c -o makeplot
	./makeplot
	open temp_log.png
