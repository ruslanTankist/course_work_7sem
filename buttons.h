#ifndef BUTTONS_H_
#define BUTTONS_H_ 1

#include <stdbool.h>

#include "byte.h"

#define BTNS_HOUR_INC	0x00
#define BTNS_HOUR_DEC	0x01
#define BTNS_MINUTE_INC	0x03
#define BTNS_MINUTE_DEC	0x04
#define BTNS_ENTER	0x05
#define BTNS_RESET	0x06


void
btns_init(void);

typedef void (*btns_intr_handler_t)(void *args);

void
btns_set_intr_handler(btns_intr_handler_t handler, void *args);

byte_t
btns_read_byte(void);

void
btns_read_byte_async(byte_t *b, bool *ready);

#endif
