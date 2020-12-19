#include <assert.h>
#include <stddef.h>

#include <avr/io.h>

#include "byte.h"
#include "max7219.h"

#include "seven_segment.h"

#define HOUR_HI		MAX7219_ADDR_DIG0
#define HOUR_LO		MAX7219_ADDR_DIG1
#define MINUTE_HI	MAX7219_ADDR_DIG2
#define MINUTE_LO	MAX7219_ADDR_DIG3
#define SECOND_HI	MAX7219_ADDR_DIG4
#define SECOND_LO	MAX7219_ADDR_DIG5

#define SEG_DATA_TO_DISPLAY_ARR_SIZE 12 /* 6 indicators + 6 addresses */

static byte_t seg_data_to_display[SEG_DATA_TO_DISPLAY_ARR_SIZE] =
	{HOUR_HI, 0, HOUR_LO, 0, MINUTE_HI, 0, MINUTE_LO, 0, SECOND_HI, 0, SECOND_LO, 0};
static size_t seg_data_to_display_i = 0;
static size_t seg_data_len = SEG_DATA_TO_DISPLAY_ARR_SIZE;
static bool *seg_display_done = NULL;

bool seg_initialized = false;

void
seg_init(void)
{
	assert(!seg_initialized);

	struct max7219_config max7219_cfg = (struct max7219_config) {
		MAX7219_ADDR_DIG5,
		MAX7219_DECODE_BITS_7_0,
		MAX7219_INTENSITY_31,
		MAX7219_DISPLAY_TEST_MODE_OFF,
	};
	max7219_init(&max7219_cfg);

	// Display initial value
	for (size_t i = 0; i < SEG_DATA_TO_DISPLAY_ARR_SIZE; i+=2) {
		max7219_write(seg_data_to_display[i], seg_data_to_display[i + 1]);
	}

	seg_initialized = true;
}

#define seg_digit_lo(_x) ((_x % 100) % 10)
#define seg_digit_hi(_x) (((_x % 100) / 10) % 10)

void
seg_display_time_props_async(struct time_props *props, bool *done)
{
	// Check if nodisplaying in progress
	assert(seg_display_done == NULL);

	seg_data_to_display[0] = HOUR_HI;
	seg_data_to_display[1] = seg_digit_hi(props->hours);
	seg_data_to_display[2] = HOUR_LO;
	seg_data_to_display[3] = seg_digit_lo(props->hours);
	seg_data_to_display[4] = MINUTE_HI;
	seg_data_to_display[5] = seg_digit_hi(props->minutes);
	seg_data_to_display[6] = MINUTE_LO;
	seg_data_to_display[7] = seg_digit_lo(props->minutes);
	seg_data_to_display[8] = SECOND_HI;
	seg_data_to_display[9] = seg_digit_hi(props->seconds);
	seg_data_to_display[10] = SECOND_LO;
	seg_data_to_display[11] = seg_digit_lo(props->seconds);

	seg_display_done = done;
	seg_data_len = SEG_DATA_TO_DISPLAY_ARR_SIZE;
	seg_data_to_display_i = 0;
}

void
seg_display_time_props_async_continue()
{
	assert(seg_display_done);
	assert(seg_data_len <= SEG_DATA_TO_DISPLAY_ARR_SIZE);

	if (seg_data_to_display_i >= seg_data_len) {
		*seg_display_done = true;
		seg_display_done = NULL;
		return;
	}

	max7219_write(seg_data_to_display[seg_data_to_display_i],
			seg_data_to_display[seg_data_to_display_i + 1]);
	seg_data_to_display_i += 2;
}
