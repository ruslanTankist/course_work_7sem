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

ISR(ADC_vect)
{
	if (adc_intr_handler) {
		adc_intr_handler();
	}
}

void
adc_set_intr_handler(adc_intr_handler_t handler)
{
	cli();

	adc_intr_handler = handler;

	sei();
}

void
adc_read_bytes(void)
{	
	struct adc_props adc_props;

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

void
adc_read_byte_async_intr_handler()
{
	adc_read_bytes();

	adc_intr_handler = NULL;
}

void
adc_read_byte_async()
{
	adc_set_intr_handler(adc_read_byte_async_intr_handler);
}

void
adc_write_eeprom (struct adc_props props)
{
	eeprom_write(props.detector_1);
	eeprom_write(props.detector_2);
}

struct adc_props *
adc_read_eeprom (byte_t *len)
{
	(*len) = eeprom_get_record_count() / 2;

	struct adc_props *adc_arr;
	byte_t j = 0;
	for(byte_t i = 0; i < (*len); i++) {
		adc_arr[i].detector_1 = eeprom_read(j);
		adc_arr[i].detector_2 = eeprom_read(j+1);

		j += 2;
	}

	return adc_arr;
}
