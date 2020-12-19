#define F_CPU		4000000L
#define BAUD_RATE	9600L

#include <assert.h>
#include <inttypes.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "buttons.h"
#include "seven_segment.h"
#include "uart.h"
#include "adc.h"
#include "time.h"
#include "spi.h"

#define handle_btn_pressed(_btn)				\
	({							\
		switch ((byte_t) _btn) {			\
		case BTNS_HOUR_INC:				\
			time_inc_hour(&new_props);		\
			break;					\
		case BTNS_HOUR_DEC:				\
			time_dec_hour(&new_props);		\
			break;					\
		case BTNS_MINUTE_INC:				\
			time_inc_minute(&new_props);		\
			break;					\
		case BTNS_MINUTE_DEC:				\
			time_dec_minute(&new_props);		\
			break;					\
		case BTNS_ENTER:				\
			cli();					\
			set = true;				\
			props.hours = new_props.hours;		\
			props.minutes = new_props.minutes;	\
			sei();					\
			break;					\
		case BTNS_RESET:				\
			cli();					\
			new_props.hours = props.hours;		\
			new_props.minutes = props.minutes;	\
			sei();					\
			break;					\
		default:					\
			assert(false);				\
		}						\
	})

// props - то, на что мы смотрим, когда считаем бизнес-логику
// new_props - то, что мы рисуем на индикаторах
// props подменяется на new_props атомарно при нажатии ENTER

int
main()
{
	cli();
//	// Internal initialization
//	uart_init(F_CPU, BAUD_RATE);
	btns_init();
	spi_init();
	adc_init();
	time_init();

	// Enable global interrupts
	sei();

	// External initialization
	seg_init();

	struct time_props props;
	struct time_props new_props;

	struct time_props props_displayed = {-1, -1};
	bool display_done = true;

	byte_t btns_b = 0;
	bool btns_ready = false;
	bool set = false;
	btns_read_byte_async(&btns_b, &btns_ready);

	struct adc_props adc_b = {0x00, 0x00};
	bool adc_ready = false;
	adc_read_byte_async(&adc_b, &adc_ready);
	start_conv_adc();

	bool time_move_ready = false;
	time_move_async(&props, &new_props);

	for (;;) {
		if (btns_ready) {
			handle_btn_pressed(btns_b);
			btns_ready = false;
			btns_read_byte_async(&btns_b, &btns_ready);
		} else if (((props_displayed.hours != new_props.hours) ||
				(props_displayed.minutes != new_props.minutes) ||
				(props_displayed.seconds != new_props.seconds)) &&
				display_done) {
			display_done = false;
			props_displayed = new_props;
			seg_display_time_props_async(&props_displayed, &display_done);
		} else if (!display_done) {
			seg_display_time_props_async_continue();
		} else if (adc_ready && set) {
			adc_ready = false;
		}
		//TODO 
		//ОТсчет времени
		//сохранять adc
	}
	return 0;
}
