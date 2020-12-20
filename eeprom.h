#ifndef EEPROM_H_
#define EEPROM_H_ 1

#include <inttypes.h>
#include "byte.h"

void
eeprom_write (byte_t data);

byte_t
eeprom_read (byte_t amount);

byte_t
eeprom_get_record_count();

#endif
