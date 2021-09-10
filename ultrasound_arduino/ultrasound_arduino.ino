
/**
 * @brief quan.h is required for arduino framework to
 *        pick up the quan library in the ../../libraries folder
*/
#include <quan.h>
#include <quan/time.hpp>
#include <quan/frequency.hpp>
#include <builtin_led.h>

namespace {
/**
* @brief These macros define quantity literals
*        eg  1_MHz 10.5_us etc
*        Note that, in use, 1_MHz deduces a quantity with an 
*        integer underlying value_type.
*        If you want a float value_type then do 1.0_MHz instead of 1_MHz
*        The macro defines both options
**/
   QUAN_QUANTITY_LITERAL(frequency, MHz)
   QUAN_QUANTITY_LITERAL(frequency, kHz)
   QUAN_QUANTITY_LITERAL(time, us)
}

namespace {
/**
 * Using the quan library, we can declare a strong C++ type 
 * representing for the system clock as a frequency.
 * Constexpr means it is a compile time constant,
 * therefore there is no runtime overhead
**/
#if defined F_CPU
 #if F_CPU == 16000000
   quan::frequency::MHz constexpr systemClockFrequency = 16_MHz;
 #else
   #error unknown cpu frequency
 #endif
#else
 #error F_CPU undefined
#endif

 quan::frequency::kHz constexpr transducerFrequency = 40_kHz;
 quan::time::us constexpr transducerCycleTime = 1.f/transducerFrequency;


/** IO setup
Net          Alt      Port PhysPin  ArdPin descr2
ZeroCrossing1 AIN0     PD6      10       6    ZeroCrossing via resistor hysteresis network
FilterGnd     AIN1     PD7      11       7    Filter Gnd 
CmpOut                 PD5       9       5    software comparator output. N.B there is no actual hardware comp output
SenseDrive    OC1B     PB2      14       10    Tim1 compare match A  
Tras0   Out            PD2      32       2    transducer address sel 0 
Tras1   Out            PD3       1       3    transducer address sel 1

**/

/**

ADCSRB.ACME = false ; // no multiplexing of comp negative input to adc pins
ACSR.ACD = false;
ACSR.ACBG = false;
ACSR.ACO = true;  // read only comparator output
ACSR.ACBG = false;
ACSR.ACI ; // comparator interrupt flag
ACSR.ACIE ; // comparator interrupt enable flag
ACSR.ACIC = true;// enable comparator as capture input to Timer1. Clear flag after mod

Set up comparator capture
ACSR.ACI0 = 0b00 ;// comparator interrupt on toggle
ACSR.ACI0 = ob01 ;// reserved
ACSR.ACI0 = 0b10 ;// comparator interrupt on Falling Output Edge
ACSR.ACI0 = 011 ;//  comparator interrupt on Rising Output Edge
**/

/**
interrrupt vector names
TIMER1_CAPT_vect
TIMER1_COMPA_vect
TIMER1_COMPB_vect
TIMER1_OVF_vect
**/

/**
 *  stop timer1
**/
void disableTIM1()
{
    // Writing TCCR1A WGM bits seems required else hangs up
    TCCR1A &= ~( (1 << WGM11) | (1<< WGM10) );
    TCCR1B &= 0b11111000;
}

/**
 *  start timer 1
**/
void enableTIM1()
{
   TCCR1B |= 1U ;
}

/*
  do pulse at start of count,
stop timer ( alternatively could get it in TOV1 interrrupt in practise
set timer to 0xffff so will roll over ASAP
pulse out on OC1B 
clear TIFR1.OCF1B
set irq TIMSK1.OIEC1B
start timer which sends pulse
on end of pulse irq
set OC1B to plain output low
set OCR1B to settle time
on settle time irq
set capture mode for pulse input
*/
void excitePulseSetup()
{
   cli();
   {
      disableTIM1();

         // set up high count count and compare reg below it on OCR1B, to prevent transition on pin when mode changed
         // also TIM will overflow soon and start pulse
         TCNT1 = 0xFFFE;

         OCR1A = 0xFFFF; // TOP
         uint16_t constexpr pulseCount = systemClockFrequency * transducerCycleTime / 2.f;
         // pulse duration to drive pulse transformer for half a cycle in microseconds
         OCR1B = pulseCount;
         //set fast pwm mode 15 OCR1A is TOP, OCR1B is comp
         TCCR1A |= (0b11 << 0U);
         TCCR1B |= (0b11 << 3U);
         // set OC1b to set at bottom, clear on match
         TCCR1A |= (0b10 << 4U);

         //disable irqs except for OC1B match  or overflow?
         // clear irq flags by writing their bits in TIFR1
         TIFR1 = 0b00100111;
        // // enable only irq on OCR1B compare in TIMER1_COMPB_vect
         TIMSK1 |= (0b1 << OCIE1B);

       enableTIM1();
   }
   sei();
}

  volatile int16_t led_count = 0;

}

#if 0
ISR (TIMER1_OVF_vect)
{
  ++led_count;
  if ( led_count > 244){
     complement_builtin_led();
     led_count = 0;
  }
}
#else
ISR (TIMER1_COMPB_vect)
{
  ++led_count;
  if ( led_count > 244){
     complement_builtin_led();
     led_count = 0;
  }
}

#endif

void setup()
{
   builtin_led_setup();
   turn_off_builtin_led();
   // set up 
   excitePulseSetup();

}

namespace {
 // int last = 0;
}

void loop()
{

}