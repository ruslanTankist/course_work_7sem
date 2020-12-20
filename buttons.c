#include <assert.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "buttons.h"

#define BTNS_PORT	PORTD		/* Register to enable pull up resistors / write data */
#define BTNS_DIR	DDRD		/* Register to specify port direction: read/write */
#define BTNS_PIN	PIND		/* Register to read data */
#define BTNS_EN		(byte_t)2	/* INT0 - any button pressed */
#define BTNS_ENC0	(byte_t)3
#define BTNS_ENC1	(byte_t)4
#define BTNS_ENC2	(byte_t)5

#define SIG_BTN_ENABLE		0
#define SIG_ENCODING_BIT0	1
#define SIG_ENCODING_BIT1	2
#define SIG_ENCODING_BIT2	3

#define BTNS_READY (!(BTNS_PIN & (1 << BTNS_EN))) /* Triggered by falling edge */

static bool btns_initialized = false;

void
btns_init(void)
{
	// Disable global interrupts
	cli();

	assert(!btns_initialized);

	// Enable interrupts by INT0
	GICR |= (1 << 6);
	// INT0 interrupt is triggerred by falling edge
	MCUCR |= (1 << 1);

	// Set all BTNS and BTNS_EV pins to input
	bit_clr(BTNS_DIR, BTNS_EN);
	bit_clr(BTNS_DIR, BTNS_ENC0);
	bit_clr(BTNS_DIR, BTNS_ENC1);
	bit_clr(BTNS_DIR, BTNS_ENC2);
	// Enable pull ups
	bit_set(BTNS_PORT, BTNS_EN);
	bit_set(BTNS_PORT, BTNS_ENC0);
	bit_set(BTNS_PORT, BTNS_ENC1);
	bit_set(BTNS_PORT, BTNS_ENC2);

	btns_initialized = true;

	// Enable global interrupts
	sei();
}

static btns_intr_handler_t btns_intr_handler = NULL;
static void *btns_intr_handler_args = NULL;

ISR(INT0_vect)
{
	if (btns_intr_handler) {
		btns_intr_handler(btns_intr_handler_args);
	}
}

void
btns_set_intr_handler(btns_intr_handler_t handler, void *args)
{
	// Disable global interrupts
	cli();

	btns_intr_handler = handler;
	btns_intr_handler_args = args;

	// Enable global interrupts
	sei();
}

byte_t
btns_read_byte(void)
{
	// Wait for user to press button
	while (!BTNS_READY);

	// Buttons are 1 by default and 0 when pressed, inverting
	byte_t btns_port_snap = ~BTNS_PIN;
	byte_t btn_props_code = 0;

	bit_copy(btns_port_snap, btn_props_code, BTNS_EN, SIG_BTN_ENABLE);
	bit_copy(btns_port_snap, btn_props_code, BTNS_ENC0, SIG_ENCODING_BIT0);
	bit_copy(btns_port_snap, btn_props_code, BTNS_ENC1, SIG_ENCODING_BIT1);
	bit_copy(btns_port_snap, btn_props_code, BTNS_ENC2, SIG_ENCODING_BIT2);
	
	return btn_props_code;
}

struct btns_read_byte_async_intr_handler_args {
	byte_t *b;
	bool *ready;
};

void
btns_read_byte_async_intr_handler(void *raw_args)
{
	assert(raw_args);

	struct btns_read_byte_async_intr_handler_args *args =
		(struct btns_read_byte_async_intr_handler_args *) raw_args;

	byte_t b = btns_read_byte();

	*(args->b) = b;
	*(args->ready) = true;

	// Clean up
	btns_intr_handler = NULL;
	btns_intr_handler_args = NULL;
}

void
btns_read_byte_async(byte_t *b, bool *ready)
{
	assert(b);
	assert(ready);

	static struct btns_read_byte_async_intr_handler_args args;
	assert(btns_intr_handler_args == NULL); // Check if async read in progress

	args = (struct btns_read_byte_async_intr_handler_args) {
		b = b,
		ready = ready,
	};

	btns_set_intr_handler(btns_read_byte_async_intr_handler, &args);
}
