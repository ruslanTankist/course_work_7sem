#ifndef SIGNAL_H_
#define SIGNAL_H_ 1

#include <stdbool.h>
#include <stddef.h>

#include "byte.h"
#include "adc.h"

void
sig_init(unsigned long cpu_freq);

#define SIG_BTN_ENABLE	0
#define SIG_ENCODING_BIT0	1
#define SIG_ENCODING_BIT1	2
#define SIG_ENCODING_BIT2	3

struct time_props {
	byte_t hours;
	byte_t minutes;
};

void
btn_time_parse_props(byte_t b, struct time_props *props);

#endif
