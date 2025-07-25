#ifndef __ADC_TEST_H
#define __ADC_TEST_H

#include <Arduino.h>

void adc_init(uint8_t pin);

uint16_t adc_read_raw(void);

float adc_read_volts(void);

uint16_t adc_read_filtered(uint8_t samples);

#endif /* __ADC_TEST_H */