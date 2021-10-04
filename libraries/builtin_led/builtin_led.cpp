
#include <Arduino.h>
#include "builtin_led.h"

/**
 On arduino pro mini builtin led is PB5
  set pin high for on , low for off
 
**/

#if !defined ARDUINO_AVR_PRO
namespace {

  bool builtinLedState = false;

  static uint8_t const builtin_led = LED_BUILTIN;

  static uint8_t const builtin_led_on = HIGH;
  static uint8_t const builtin_led_off = LOW;

}

bool get_builtin_led_state()
{
  return  builtinLedState;
}
/**
  since this is used in irq, the standard digital write is way too slow
**/

void turn_on_builtin_led()
{
   builtinLedState= true;
   digitalWrite(builtin_led,builtin_led_on);

}

void turn_off_builtin_led()
{

   builtinLedState= false;
   digitalWrite(builtin_led,builtin_led_off);

}

void builtin_led_setup()
{
  pinMode(builtin_led, OUTPUT);
  turn_on_builtin_led();
}

void complement_builtin_led()
{
   if ( builtinLedState == false){
     turn_on_builtin_led();
   }else{
      turn_off_builtin_led();
   }
}
#endif





