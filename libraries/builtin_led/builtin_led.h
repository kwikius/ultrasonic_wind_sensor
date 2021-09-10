#ifndef BUILIN_LED_H_INCLUDED
#define BUILIN_LED_H_INCLUDED

#include <Arduino.h>

void builtin_led_setup();
void complement_builtin_led();
void turn_on_builtin_led();
void turn_off_builtin_led();
bool get_builtin_led_state();

#endif // BUILIN_LED_H_INCLUDED
