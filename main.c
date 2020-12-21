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
#include "crc8.h"

#define CONFIRM_REQUEST 0x01
#define DENY_BYTE 0x00
#define DEFAULT_BYTE 0xFF

#define time_differs(_time1, _time2)		\
	((_time1.hours != _time2.hours) || (_time1.minutes != _time2.minutes) || (_time1.seconds != _time2.seconds))

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
	// Internal initialization
	btns_init();
	spi_init();
	adc_init();
	time_init(F_CPU);

	uart_init(F_CPU, BAUD_RATE);
	// Enable global interrupts
	sei();

	// External initialization
	seg_init();

	struct time_props props = {0, 0, 0};
	struct time_props new_props = props;

	struct time_props props_displayed = {-1, -1};
	bool display_done = true;

	byte_t btns_b = 0;
	bool btns_ready = false;
	bool set = false;
	btns_read_byte_async(&btns_b, &btns_ready);

	bool read_eeprom_once = true;
	bool write_eeprom_once = true;

	bool time_to_write = false;
	bool time_to_read_and_send = false;
	time_move_async(&props, &new_props);

	byte_t confirm_byte = DEFAULT_BYTE;
	byte_t uart_byte = DEFAULT_BYTE;
	bool uart_ready = false;
	bool uart_confirmation = false;
	bool repeated_request = false;

	for (;;) {
		// event conditions to work with eeprom
		if ((props.minutes % 30) == 0)
			time_to_write = true;
		else
			time_to_write = false;

		if ((props.hours % 12) == 0)
			time_to_read_and_send = true;
		else
			time_to_read_and_send = false;

		if (btns_ready) {
			handle_btn_pressed(btns_b);
			btns_ready = false;
			btns_read_byte_async(&btns_b, &btns_ready);
		} else if (time_differs(props_displayed, new_props) && display_done) {
			display_done = false;
			props_displayed = new_props;
			seg_display_time_props_async(&props_displayed, &display_done);
		} else if (!display_done) {
			seg_display_time_props_async_continue();
		} else if (time_to_write && set && write_eeprom_once) {
			write_eeprom_once = false;
			adc_read_bytes(props);
		} else if (!time_to_write) {
			write_eeprom_once = true;
		} else if ((time_to_read_and_send && set && read_eeprom_once) || repeated_request) {
			read_eeprom_once = false;
			uart_write_byte(CONFIRM_REQUEST);

			uart_must_read_byte_async(&uart_byte, &uart_ready);
		} else if (uart_ready) {
			uart_ready = false;
			confirm_byte = uart_byte;
			if (confirm_byte != DENY_BYTE && confirm_byte != DEFAULT_BYTE) {
				uart_confirmation = true;
				confirm_byte = DEFAULT_BYTE;
				repeated_request = false;
			} else if (confirm_byte == DENY_BYTE) {
				repeated_request = true;
			}
		} else if (uart_confirmation) {
			uart_confirmation = false;
			byte_t len;
			struct adc_props adc_arr[48];
			adc_read_eeprom(adc_arr, &len);
			for (byte_t i = 0; (i < len) && (i < 48) ; i++) {
				uart_write_byte(adc_arr[i].detector_1);
				uart_write_byte(adc_arr[i].detector_2);
				uart_write_byte(adc_arr[i].time.hours);
				uart_write_byte(adc_arr[i].time.minutes);
			}
			uart_write_byte(get_crc(&(adc_arr[0].detector_1), (len*4)));
		} else if (!time_to_read_and_send) {
			read_eeprom_once = true;
		}
	}
	return 0;
}
