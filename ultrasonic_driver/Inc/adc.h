#ifndef STM32L4_V1_ADC_H_INCLUDED
#define STM32L4_V1_ADC_H_INCLUDED

#include <quan/voltage.hpp>

void adc_setup();

quan::voltage_<float>::V read_adc();

bool have_new_adc_conversion();

void clear_new_adc_conversion();

#endif // STM32L4_V1_ADC_H_INCLUDED
