SOURCES = ds1620.c
ARCHIVE = libDS1620.a
AVR_CC = avr-gcc
AVR_ARCHIVE = avr-ar
AVR_CFLAGS = -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL
AVR_AFLAGS = -r
AVR_OBJECTS = $(SOURCES:.c=.o)

all: AVR_BUILD
	
AVR_BUILD: $(ARCHIVE)

$(ARCHIVE) : $(AVR_OBJECTS)
	$(AVR_ARCHIVE) $(AVR_AFLAGS) $@ $<
	rm -f $<

%.o: %.c
	$(AVR_CC) $(AVR_CFLAGS) -c $<

clean:
	rm -f $(AVR_OBJECTS) $(PROGRAM) $(ARCHIVE)
	