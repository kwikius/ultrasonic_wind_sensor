#ifndef STM32L4_V1_RESOURCE_HPP_INCLUDED
#define STM32L4_V1_RESOURCE_HPP_INCLUDED

#include <quan/stm32/gpio.hpp>
#include <quan/stm32/tim.hpp>

typedef quan::mcu::pin<quan::stm32::gpioa,2> txo_pin;// (A7)
typedef quan::mcu::pin<quan::stm32::gpioa,15> rxi_pin; //PA15 not broken out on Nucleo-L432 N.B not used ATM

typedef quan::mcu::pin<quan::stm32::gpiob,0> analog_in;
typedef quan::mcu::pin<quan::stm32::gpiob,3> led;
using pulse_stream_out_pin = quan::mcu::pin<quan::stm32::gpioa,8>; //(D9) TIM1_CH1 AF1

// want 2 TIM2 capture inputs and address outputs
// PA0  TIM2_CH1  AF1  EnvelopeDetect
// PA1  TIM2_CH2  AF1  ZeroCrossing

//todo swap names of these two as that is how they are in practise
// must be 5v tolerant - yes
using envelope_detect_pin = quan::mcu::pin<quan::stm32::gpioa,0>; // (A0) TIM2_CH1 AF1
// must be 5v tolerant -
using zero_crossing_pin = quan::mcu::pin<quan::stm32::gpioa,1>;  // (A1)  TIM2_CH2 AF1

//PB4 Address0
//PB5 Address1

// must be 5v tolerant open collector
using ultrasound_address0_pin = quan::mcu::pin<quan::stm32::gpiob,6>; // (D5)
// must be 5v tolerant open collector
using ultrasound_address1_pin = quan::mcu::pin<quan::stm32::gpiob,7>;// (D4)

typedef quan::stm32::tim6 adc_timer;
typedef quan::stm32::tim15 dummy_timer;
using tof_timer = quan::stm32::tim2;
using pulse_timer = quan::stm32::tim1;

#endif // STM32L4_V1_RESOURCE_HPP_INCLUDED
