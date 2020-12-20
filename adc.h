#ifndef ADC_H_
#define ADC_H_ 1

#include <stdbool.h>
#include "byte.h"

#define start_conv_adc() ({bit_set(ADCSRA, ADSC);})
#define stop_conv_adc() ({bit_clr(ADCSRA, ADSC);})

void
adc_init(void);

struct adc_props {
	byte_t detector_1;
	byte_t detector_2;
};

typedef void (*adc_intr_handler_t)();

void
adc_set_intr_handler(adc_intr_handler_t handler);

void
adc_read_bytes(void);

void
adc_read_byte_async();

void
adc_write_eeprom (struct adc_props props);

struct adc_props *
adc_read_eeprom (byte_t *len);

#endif
