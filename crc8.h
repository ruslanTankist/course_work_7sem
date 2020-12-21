#ifndef CRC8_H_
#define CRC8_H_ 1

#include <stdlib.h>

#include "adc.h"
#include "byte.h"

byte_t
crc8 (byte_t crc, unsigned char const *data, size_t len);

byte_t
crc8_slow (byte_t crc, unsigned char const *data, size_t len);

byte_t
get_crc (unsigned char *buf, byte_t chunk_len);

#endif
