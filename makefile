SRC := ds1620.c
ARCHIVE := $(addprefix lib,$(SRC:.c=.a))
AVR_MMCU = atmega328p
AVR_CPU_SPEED=16000000UL

CROSS_COMPILE := avr-
CC := gcc
AR := ar

AVR_CFLAGS := -Wall -g2 -gstabs -O1 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=$(AVR_MMCU) -DF_CPU=$(AVR_CPU_SPEED)
AVR_AFLAGS := -r
AVR_OBJECTS := $(SOURCES:.c=.o)

all: AVR_BUILD

AVR_BUILD: $(ARCHIVE)

$(ARCHIVE) : $(AVR_OBJECTS)
	$(CROSS_COMPILE)$(AR) $(AVR_AFLAGS) $@ $<
	rm -f $<

%.o: %.c
	$(CROSS_COMPILE)$(CC) $(AVR_CFLAGS) -c $<

clean:
	rm -f $(AVR_OBJECTS) $(PROGRAM) $(ARCHIVE)
