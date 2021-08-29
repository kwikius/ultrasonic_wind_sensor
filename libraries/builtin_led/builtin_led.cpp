
#include "Arduino.h"

namespace {

  String builtinLedState = "off";

  static uint8_t const builtin_led = LED_BUILTIN;
  static uint8_t const builtin_led_on = LOW;
  static uint8_t const builtin_led_off = HIGH;

}

String get_builtin_led_state()
{
  return  builtinLedState;
}

void turn_on_builtin_led()
{
   builtinLedState= "on";
   digitalWrite(builtin_led,builtin_led_on);
}

void turn_off_builtin_led()
{
   builtinLedState= "off";
   digitalWrite(builtin_led,builtin_led_off);
}

void builtin_led_setup()
{
  pinMode(builtin_led, OUTPUT);
  turn_on_builtin_led();
}

void complement_builtin_led()
{
   if ( builtinLedState == "off"){
     turn_on_builtin_led();
   }else{
      turn_off_builtin_led();
   }
}





