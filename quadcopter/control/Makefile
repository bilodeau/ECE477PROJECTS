CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega328p -Wno-char-subscripts
OBJ2HEX=avr-objcopy
#UISP=/usr/local/bin/uisp
TARGET=master
TEST=test
SLAVE=../i2c/slave
ADFLAGS=-p atmega328p -c avrispmkII -P usb

.PHONY: fuses prog erase monitor status slave test


prog : $(TARGET).hex
	avrdude $(ADFLAGS) -V -U flash:w:$(TARGET).hex:i

%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@ -lm

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

erase :
	 avrdude $(ADFLAGS) -e
clean :
	rm -f *.hex *.obj *.o *.eeprom

fuses:
	avrdude $(ADFLAGS) -U lfuse:w:0xe2:m
	avrdude $(ADFLAGS) -U hfuse:w:0xd9:m
	avrdude $(ADFLAGS) -U efuse:w:0xff:m

monitor:
	gcc ../lib/PCmonitor.c -o monitor -lncurses

status:
	gcc ../lib/PCstatus.c -o status -lncurses

slave: $(SLAVE).hex
	avrdude $(ADFLAGS) -V -U flash:w:$(SLAVE).hex:i

test: $(TEST).hex
	avrdude $(ADFLAGS) -V -U flash:w:$(TEST).hex:i
