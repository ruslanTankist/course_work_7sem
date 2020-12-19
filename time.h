#ifndef TIME_H_
#define TIME_H_ 1

#include "byte.h"

void
time_init(void);

struct time_props {
	byte_t hours;
	byte_t minutes;
	byte_t seconds;
};

typedef void (*time_intr_handler_t)(void *args);

void
time_move_async(struct time_props *time_setting, struct time_props *new_time_setting);

void
time_inc_hour(struct time_props *time_setting);

void
time_dec_hour(struct time_props *time_setting);

void
time_inc_minute(struct time_props *time_setting);

void
time_dec_minute(struct time_props *time_setting);

void
time_inc_second(struct time_props *time_setting);

void
time_dec_second(struct time_props *time_setting);

#endif
