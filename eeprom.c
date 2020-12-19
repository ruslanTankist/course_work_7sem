#include "eeprom.h"

#include <avr/eeprom.h>

static uint32_t current_address = 0;

void
eeprom_write (byte_t data)
{
	bit_clr(&EECR, EEWE);
	EEAR = current_address;
	EEDR = data;
	bit_set(&EECR, EEMWE);
	bit_set(&EECR, EEWE);
}

byte_t
eeprom_read (byte_t address)
{
	EEAR = address;
	bit_set(&EECR, EERE);
	byte_t data = EEDR;

	return data;
}

uint32_t
eeprom_get_record_count()
{
	return current_address;
}
