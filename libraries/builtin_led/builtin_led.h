#ifndef BUILIN_LED_H_INCLUDED
#define BUILIN_LED_H_INCLUDED

#include <Arduino.h>

#if defined ARDUINO_AVR_PRO

inline void builtin_led_setup()
{
     PORTB &= ~(0b1 << 5U);
     DDRB  |= (0b1 << 5U);
     PORTB |= (0b1 << 5U);
}

inline void turn_on_builtin_led()
{
   PORTB |= (0b1 << 5U);
}

inline void turn_off_builtin_led()
{
   PORTB &= ~(0b1 << 5U);
}

inline bool get_builtin_led_state()
{
   return (PINB & (0b1 << 5U)) != 0U;
}

inline void complement_builtin_led()
{
   PINB |= (0b1 << 5U);
} 

#else

void builtin_led_setup();
void turn_on_builtin_led();
void turn_off_builtin_led();
bool get_builtin_led_state();
void complement_builtin_led();

#endif // ARDUINO_AVR_PRO

#endif // BUILIN_LED_H_INCLUDED
