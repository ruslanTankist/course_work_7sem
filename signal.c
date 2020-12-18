#include <assert.h>

#include <avr/interrupt.h>

#include "signal.h"

#define T0_MAX_TACTS 256

#define SIG_PORT	PORTD	/* Register to enable pull up resistors / write data */
#define SIG_DIR		DDRD	/* Register to specify port direction: read/write */
#define SIG_PIN		PIND	/* Register to read data */

static bool sig_initialized = false;
static unsigned long sig_tfreq = 0; // Timer frequency

void
sig_init(unsigned long cpu_freq)
{
	// Disable global interrupts
	cli();

	assert(!sig_initialized);

	// We are using frequency divider
	sig_tfreq = cpu_freq / 8;

	// Set frequency divider cpu_freq / 8
	TCCR0 = 0;
	bit_set(&TCCR0, CS01);
	// Enable T0 interrupts
	bit_set(&TIMSK, TOIE0);

	sig_initialized = true;

	// Enable global interrupts
	sei();
}

void
sig_parse_props(byte_t b, struct btn_props *props)
{
	switch (b) {
	case 0x00:
		// UNDEFINED
		break;
	case 0x01:
		// + HOUR
		break;
	case 0x03:
		// - HOUR
		break;
	case 0x05:
		// + MINUTE
		break;
	case 0x07:
		// - MINUTE
		break;
	case 0x09:
		// ENTER
		break;
	case 0x0B:
		// TIME RESET
		break;
	}
}

