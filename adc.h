#ifndef ADC_H_
#define ADC_H_ 1

#include <stdbool.h>

#define StartConvAdc()  ADCSRA |= (1<<ADSC)

void 
adc_init();

typedef void (*adc_intr_handler_t)(void *args);

#endif