#ifndef BYTE_H_
#define BYTE_H_ 1

#include <stdbool.h>
#include <inttypes.h>

#define NULL ((void *)0)

typedef volatile uint8_t byte_t;

#define byte_hi(_x)	((byte_t)(_x >> 8))
#define byte_lo(_x)	((byte_t)(_x & 0xFF))

#define bit_set(_dest, _bit) ({_dest |= (1 << _bit);})
#define bit_clr(_dest, _bit) ({_dest &= (~(1 << _bit));})

#define bit_get(_src, _bit) (_src & (1 << _bit))
#define bit_invert(_dest, _bit)						\
	({								\
		if(bit_get(_dest, _bit)) {				\
			bit_clr(_dest, _bit);				\
		} else {						\
			bit_set(_dest, _bit);				\
		}							\
	 })
#define bit_def(_dest, _bit, _val)					\
	({								\
		if (_val) {						\
			bit_set(_dest, _bit);				\
		} else {						\
			bit_clr(_dest, _bit);				\
		}							\
	})
#define bit_copy(_src, _dest, _src_bit, _dest_bit)			\
	({								\
		byte_t _tmp = bit_get(_src, _src_bit);			\
		bit_def(_dest, _dest_bit, _tmp);			\
	})

#endif
