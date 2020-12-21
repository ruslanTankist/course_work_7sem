#include "eeprom.h"

#include <avr/eeprom.h>
#include <avr/interrupt.h>

static byte_t current_address = 0;

void
eeprom_write (byte_t data)
{
	cli();
	eeprom_write_byte((uint8_t*)current_address, data);
	sei();
	if(current_address<95)
		current_address++;
}

byte_t
eeprom_read (byte_t address)
{
	byte_t b = eeprom_read_byte((uint8_t*)address);

	// all record for 12 hours are read, we clear them
	if((address+1) == current_address)
		current_address = 0;
	return b;
}

byte_t
eeprom_get_record_count()
{
	return current_address/4;
}
