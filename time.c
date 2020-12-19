#include "time.h"
#include <assert.h>

#include <avr/interrupt.h>

#include "byte.h"

void
time_init(void)
{
	// pre-scalar 1024
	bit_set(&TCCR1B, CS10);
	// enable T1 interrupt
	bit_set(&TIMSK, TOIE1);

	OCR1A = 3906;
	TCNT1 = 0;
}

static time_intr_handler_t time_intr_handler = NULL;
static void *time_intr_handler_args = NULL;

ISR(TIMER1_OVF_vect)
{
	if (time_intr_handler) {
		time_intr_handler(time_intr_handler_args);
	}
	TCNT1 = 0;
	bit_set(&TIFR, OCF1A);
}

void
time_set_intr_handler(time_intr_handler_t handler, void *args)
{
	cli();

	time_intr_handler = handler;
	time_intr_handler = args;

	sei();
}

struct time_move_async_intr_handler_args {
	struct time_props *time_setting;
	bool *ready;
};

void
time_move(struct time_props *time_setting)
{
	time_inc_second(time_setting);
}

void
time_move_async_intr_handler(void *raw_args)
{
	assert(raw_args);

	struct time_move_async_intr_handler_args *args =
		(struct time_move_async_intr_handler_args *) raw_args;

	time_move(&(args->time_setting));

	*(args->ready) = true;

	time_intr_handler = NULL;
	time_intr_handler_args = NULL;
}

void
time_move_async(struct time_props *time_setting, bool *ready)
{
	assert(ready);

	static struct time_move_async_intr_handler_args args;
	assert(time_intr_handler_args == NULL);

	args = (struct time_move_async_intr_handler_args) {
		time_setting = time_setting,
		ready = ready,
	};

	time_set_intr_handler(time_move_async_intr_handler, &args);
}

void
time_inc_hour(struct time_props *time_setting)
{
	if (time_setting->hours < 23)
		time_setting->hours++;
	else
		time_setting->hours = 0;
}

void
time_dec_hour(struct time_props *time_setting)
{
	if (time_setting->hours > 0)
		time_setting->hours--;
	else
		time_setting->hours = 23;
}

void
time_inc_minute(struct time_props *time_setting)
{
	if (time_setting->minutes < 59) {
		time_setting->minutes++;
	} else {
		time_setting->minutes = 0;
		time_inc_hour(time_setting);
	}
}

void
time_dec_minute(struct time_props *time_setting)
{
	if (time_setting->minutes > 0) {
		time_setting->minutes--;
	} else {
		time_setting->minutes = 59;
		time_dec_hour(time_setting);
	}
}

void
time_inc_second(struct time_props *time_setting)
{
	if (time_setting->seconds < 59) {
		time_setting->seconds++;
	} else {
		time_setting->seconds = 0;
		time_inc_minute(time_setting);
	}
}

void
time_dec_second(struct time_props *time_setting)
{
	if (time_setting->seconds > 0) {
		time_setting->seconds--;
	} else {
		time_setting->seconds = 59;
		time_dec_minute(time_setting);
	}
}
