#include <assert.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "adc.h"
#include "eeprom.h"

#define PIN_ADC0 0x60
#define PIN_ADC1 0x61

static bool adc_initialized = false;

void
adc_init()
{
	assert(!adc_initialized);

	//ион - напряжение питания, выравнивание влево, нулевой канал
	ADMUX = (0<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0);

	//вкл. ацп, режим одиночного преобр., разрешение прерывания,частота преобр. = FCPU/128
	ADCSRA = (1<<ADEN)|(0<<ADSC)|(0<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

	adc_initialized = true;
}

static adc_intr_handler_t adc_intr_handler = NULL;
static void *adc_intr_handler_args = NULL;

ISR(ADC_vect)
{
	if (adc_intr_handler) {
		adc_intr_handler(adc_intr_handler_args);
	}
}

void
adc_set_intr_handler(adc_intr_handler_t handler, void *args)
{
	cli();

	adc_intr_handler = handler;
	adc_intr_handler_args = args;

	sei();
}

void
adc_read_bytes(struct time_props time)
{
	struct adc_props adc_props;
	adc_props.time = time;

	ADMUX = PIN_ADC0;
	start_conv_adc();
	while (ADCSRA & (1 << ADSC));
	adc_props.detector_1 = ADCH;

	ADMUX = PIN_ADC1;
	start_conv_adc();
	while (ADCSRA & (1 << ADSC));
	adc_props.detector_2 = ADCH;

	adc_write_eeprom(adc_props);
}

struct adc_read_byte_async_intr_handler_args {
	struct time_props time;
};

void
adc_read_byte_async_intr_handler(void *raw_args)
{
	struct adc_read_byte_async_intr_handler_args *args =
		(struct adc_read_byte_async_intr_handler_args *) raw_args;

	adc_read_bytes(args->time);

	adc_intr_handler = NULL;
	adc_intr_handler_args = NULL;
}

void
adc_read_byte_async(struct time_props time)
{
	static struct adc_read_byte_async_intr_handler_args args;
	assert(adc_intr_handler_args = NULL);

	args = (struct adc_read_byte_async_intr_handler_args) {
		time = time,
	};

	adc_set_intr_handler(adc_read_byte_async_intr_handler, &args);
}

void
adc_write_eeprom (struct adc_props props)
{
	eeprom_write(props.detector_1);
	eeprom_write(props.detector_2);
	eeprom_write(props.time.hours);
	eeprom_write(props.time.minutes);
}

void
adc_read_eeprom (struct adc_props *adc_arr, byte_t *len)
{
	(*len) = eeprom_get_record_count();

	byte_t j = 0;
	for(byte_t i = 0; i < (*len); i++) {
		adc_arr[i].detector_1 = eeprom_read(j);
		adc_arr[i].detector_2 = eeprom_read(j+1);
		adc_arr[i].time.hours = eeprom_read(j+2);
		adc_arr[i].time.minutes = eeprom_read(j+3);

		j += 4;
	}
}
