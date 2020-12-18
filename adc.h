#ifndef ADC_H_
#define ADC_H_ 1

#include <stdbool.h>
#include "byte.h"

#define start_conv_adc()  ({ADCSRA |= (1<<ADSC);})

void 
adc_init(void);

struct adc_props {
	byte_t detector_1;
	byte_t detector_2;
};

typedef void (*adc_intr_handler_t)(void *args);

void
adc_set_intr_handler(adc_intr_handler_t handler, void *args);

struct adc_props
adc_read_bytes(void);

void
adc_read_byte_async(struct adc_props *b, bool *ready);

#endif
