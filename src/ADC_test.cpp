#include "ADC_test.h"

static uint8_t adc_pin = 0;
static const float ref_voltage = 3.3f;

/* Initialize the adc test module */
void adc_init(uint8_t pin)
{
    adc_pin = pin;
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    pinMode(adc_pin, INPUT);
}

/* Read the adc raw value */
uint16_t adc_read_raw(void)
{
    return analogRead(adc_pin);
}

/* Read the adc voltage */
float adc_read_volts(void)
{
    return analogReadMilliVolts(adc_pin);
}



uint16_t adc_read_filtered(uint8_t samples)
{
    if (samples == 0)
        samples = 1;
    uint32_t sum = 0;

    for (uint8_t i = 0; i < samples; i++)
    {
        sum += adc_read_raw();
        delayMicroseconds(100);
    }
    return (uint16_t)(sum / samples);
}