#include <assert.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "adc.h"

static bool adc_initialized = false;

void
adc_init()
{
	assert(!adc_initialized);
	
	//ион - напряжение питания, выравнивание влево, нулевой канал
	ADMUX = (0<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0);
	
	//вкл. ацп, режим одиночного преобр., разрешение прерывания,частота преобр. = FCPU/128
	ADCSRA = (1<<ADEN)|(0<<ADSC)|(0<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

ISR(ADC_vect)
{
	unsigned char AdcBuf = ADCH;
	
	if (AdcBuf > 240)
		KeyBuf = KEY_S4;
	else if (AdcBuf > 180)
		KeyBuf = KEY_S3;
	else if (AdcBuf > 120)
		KeyBuf = KEY_S2;
	else if (AdcBuf > 35)
		KeyBuf = KEY_S1;
	else
		KeyBuf = KEY_NULL;
	
	StartConvAdc();
}