#define F_CPU		4000000L
#define BAUD_RATE	9600L

#include <assert.h>
#include <inttypes.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "uart.h"
#include "adc.h"

#define handle_btn_pressed (_btn)				\
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
			time_inc_minute(&new_props);		\
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
//	uart_init(F_CPU, BAUD_RATE);

	btns_init();

//	spi_init();

	// Enable global interrupts
	sei();

	seg_init();

	struct time_props props;
	struct time_props new_props;

	byte_t btns_b = 0;
	bool btns_ready = false;
	bool set = false
	btns_read_byte_async(&btns_b, &btns_ready);

	// wait until the time will be set
	while (!set) {
		if (btns_ready) {

			new_props_byte = btns_b;
			btns_ready = false;
			btns_read_byte_async(&btns_b, &btns_ready);
		} else if (props_displayed_byte != time_props && display_done) {
			display_done = false;
			props_displayed_byte = props_byte;
			time_parse_props(props_displayed_byte, &props_displayed);
			seg_display_time_props_async(&props_displayed, &display_done);
		} else if (!display_done) {
			seg_display_time_props_async_continue();
		}
		//props_byte -> btn_code
		//
	}

	adc_init();

	byte_t adc_b = 0;
	bool adc_ready = false;
	adc_read_byte_async(&adc_b, &adc_ready);
	start_conv_adc();

	for (;;) {
		if (new_time_props != time_props) {
			time_props = new_time_props;
		} else if (btns_ready) {
			new_time_props = btns_b;
			btns_ready = false;
			btns_read_byte_async(&btns_b, &btns_ready);
		} else if (props_displayed_byte != time_props && display_done) {
			display_done = false;
			props_displayed_byte = props_byte;
			time_parse_props(props_displayed_byte, &props_displayed);
			seg_display_time_props_async(&props_displayed, &display_done);
		} else if (!display_done) {
			seg_display_time_props_async_continue();
		}
		//TODO 
		//ОТсчет времени
		//сохранять adc
	}
	return 0;
 }
