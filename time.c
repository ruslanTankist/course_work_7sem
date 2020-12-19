#include "time.h"
#include <assert.h>

#include <avr/interrupt.h>

#include "byte.h"

#define ONE_SEC_TACTS 61632

void
time_init(void)
{
	// pre-scalar 1024
	bit_set(&TCCR1B, CS10);
	bit_set(&TCCR1B, CS12);
	// enable T1 interrupt
	bit_set(&TIMSK, TOIE1);

	TCNT1 = ONE_SEC_TACTS;
}

static struct time_props *time_setting_loc = NULL;
static struct time_props *new_time_setting_loc = NULL;

ISR(TIMER1_OVF_vect)
{
	time_inc_second(time_setting_loc);
	time_inc_second(new_time_setting_loc);
	TCNT1 = ONE_SEC_TACTS;
	bit_set(&TIFR, OCF1A);
}

void
time_move_async(struct time_props *time_setting, struct time_props *new_time_setting)
{
	time_setting_loc = time_setting;
	new_time_setting_loc = new_time_setting;
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
