#define F_CPU		4000000L
#define BAUD_RATE	9600L

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "uart.h"
#include "adc.h"

int main()
 { 
//	uart_init(F_CPU, BAUD_RATE);

//	btns_init();

//	spi_init();
	 
	adc_init();

	byte_t adc_b = 0;
	bool adc_ready = false;
	adc_read_byte_async(&adc_b, &adc_ready);
	// Enable global interrupts
	sei();
	 
/*	 for(;;) {
		 // TODO: read time with uart
	 }
*/
	 
	StartConvAdc();

	for (;;) {
		
	}
	return 0;
 }
