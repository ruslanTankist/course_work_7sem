#ifndef ADC_H_
#define ADC_H_ 1

#include <stdbool.h>
#include "byte.h"
#include "time.h"

#define start_conv_adc() ({bit_set(ADCSRA, ADSC);})
#define stop_conv_adc() ({bit_clr(ADCSRA, ADSC);})

void
adc_init(void);

struct adc_props {
	byte_t detector_1;
	byte_t detector_2;
	struct time_props time;
};

typedef void (*adc_intr_handler_t)(void *args);

void
adc_set_intr_handler(adc_intr_handler_t handler, void *args);

void
adc_read_bytes(struct time_props time);

void
adc_read_byte_async(struct time_props time);

void
adc_write_eeprom (struct adc_props props);

void
adc_read_eeprom (struct adc_props *adc_arr, byte_t *len);

#endif
