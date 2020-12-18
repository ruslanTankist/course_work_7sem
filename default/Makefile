###############################################################################
# Makefile for the project yuzeev_course
###############################################################################

## General Flags
PROJECT = yuzeev_course
MCU = atmega8535
TARGET = yuzeev_course.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -Os -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=yuzeev_course.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = adc.o buttons.o byte.o main.o max7219.o seven_segment.o signal.o spi.o uart.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) yuzeev_course.hex yuzeev_course.eep yuzeev_course.lss size

## Compile
adc.o: ../adc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

buttons.o: ../buttons.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

byte.o: ../byte.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

max7219.o: ../max7219.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

seven_segment.o: ../seven_segment.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

signal.o: ../signal.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

spi.o: ../spi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart.o: ../uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) yuzeev_course.elf dep/* yuzeev_course.hex yuzeev_course.eep yuzeev_course.lss yuzeev_course.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

