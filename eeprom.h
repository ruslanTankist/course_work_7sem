#ifndef EEPROM_H_
#define EEPROM_H_ 1

#include "byte.h"

void
eeprom_write (byte_t data);

byte_t
eeprom_read (byte_t amount);

uint32_t
eeprom_get_record_count();

#endif
