
/**
 * @brief quan.h is required for arduino framework to
 *        pick up the quan library in the ../../libraries folder
*/
#include <quan.h>
#include <quan/time.hpp>
#include <quan/frequency.hpp>
#include <builtin_led.h>

// for showing output at human visible speed
#define UWS_SLOW_MOTION_DEBUG
#define UWS_EXTERNAL_COMPARATOR

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
   QUAN_QUANTITY_LITERAL(time, ms)

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

/**
 * @brief ultrasonic transducer resonant frequency
**/
#if defined UWS_SLOW_MOTION_DEBUG
 quan::frequency::kHz constexpr transducerFrequency = 10_kHz;
#else
 quan::frequency::kHz constexpr transducerFrequency = 40_kHz;
#endif
 quan::time::us constexpr transducerCycleTime = 1.f/transducerFrequency;

/**
*  @brief Time from start of pulse to end of delay after tx pulse is sent for the filter to settle
*  @todo Since we only use one pulse, it could be shortened to reduce amplitude of the
*  output pulse, which should reduce the settling delay. 
*  (Currently pulse is transducerCycleTime/2)
**/
 quan::time::us constexpr txSettlingDelayEnd = 1_ms;

 /**
  * @brief time from TCNT1 == 0 to next TCNT1 == 0
  **/
 quan::time::us constexpr tim1Period = 0x10000 / systemClockFrequency;

 /**
  * @brief overestimate of time required to setup next tx pulse before TIM1 overflow
  **/ 

#if defined UWS_SLOW_MOTION_DEBUG
  // to make pulse visible in slow motion
  uint16_t constexpr txSetupCycles = 2000;
#else
 uint16_t constexpr txSetupCycles = 200;
#endif

/**
*  @brief time in TIM1 period at which to start setting up for next tx Pulse
**/
 quan::time::us constexpr txSetupStart = tim1Period - txSetupCycles / systemClockFrequency;

/** IO setup
Net           Alt      Port PhysPin  ArdPin descr2
if external comparator{
RxDetect      ICP1     PB0      12       D8   
}else{
ZeroCrossing1 AIN0     PD6      10       D6    ZeroCrossing via resistor hysteresis network
FilterGnd     AIN1     PD7      11       D7    Filter Gnd 
}
CmpOut                 PD5       9       D5    software comparator output. N.B there is no actual hardware comp output
SenseDrive    OC1B     PB2      14       D10    Tim1 compare match A  
Tras0   Out            PC2      25       A2    transducer address sel 0 
Tras1   Out            PC3      26       A3    transducer address sel 1

**/

/**
 *  @brief stop timer1
**/
void disableTIM1()
{
    // Writing TCCR1A WGM bits seems required else hangs up
    TCCR1A &= ~((1 << WGM11) | (1<< WGM10));
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10) );
}

/**
 *  start timer 1
**/
void enableTIM1()
{
#if defined UWS_SLOW_MOTION_DEBUG
   TCCR1B |= ((1 << CS12) | (1 << CS10)) ; // div 1024 use to slow down the pulse for testing
#else
   TCCR1B |= (1 << CS10) ; // div 1
#endif
}

/**
 * TIM1 state machine
**/
enum class TIM1mode : uint8_t {
    undefined,
    preTXpulse,
    inTXpulse,
    inTXpulseSettlingDelay,
    inRXcapture
};

volatile auto TIM1state = TIM1mode::undefined;

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

/**
 * @brief set OCR1B to give a compare event at t
 * called in irq
 * @note make sure that t is constexpr
**/
inline void setOCR1B(quan::time::us const & t)
{
   OCR1B = systemClockFrequency * t - 1U;
}
/**
* @brief usually called in interrupt.
* Setup for next tx pulse
**/
void txPulseSetup()
{
   setOCR1B(transducerCycleTime / 2.f);
   // set OC1B to set at BOTTOM, clear on pulseCount match
   TCCR1A |= (0b10 << COM1B0);
   // clear irq flags (N.B. write a 1 to bit to clear flag)
   TIFR1 = 0b00100111;
   // enable the Pulse start and end interrrupts
   // pulse starts at TIM1 overflow
   // ends on OCR1B compare
   TIMSK1 |= (0b1 << OCIE1B) | (0b1 << TOIE1);

   TIM1state = TIM1mode::preTXpulse;
}

void txPulseInitialSetup()
{
   cli();
   {
      disableTIM1();
         //set up  OC1B(PB2) as low output 
         
         PORTB &= ~(1U << 2U);
         DDRB  |= (1U << 2U);
        
         // set up high count count and compare reg below it on OCR1B, to prevent transition on pin when mode changed
         // also TIM will overflow soon and start pulse
         TCNT1 = 0xFFFE;
         
         OCR1A = 0xFFFF; // TOP
         //set fast pwm mode 15 OCR1A is TOP, OCR1B is comp
         TCCR1A |= (0b11 << 0U);
         TCCR1B |= (0b11 << 3U);
         txPulseSetup();
        
       enableTIM1();  // to start
   }
   sei();
}

} // ~namespace

/**
* Overflow at rise of Tx pulse when TCNT1 overflows
* 
**/
ISR (TIMER1_OVF_vect)
{
   switch (TIM1state){
      case TIM1mode::preTXpulse:
         TIM1state = TIM1mode::inTXpulse;
         break;
      default:
         break;
   }
}
/**
 * --- states ---
 * sendingTxPulse
 * txPulseSettlingDelay
 * setupCapture
 * getCaptureOrTimeout
 * rxSettlingDelay
**/

ISR (TIMER1_COMPB_vect)
{
   switch(TIM1state){
      case TIM1mode::inTXpulse:
         /// @todo switch to rx address
         // change TX pulse pin output mode to normal
         TCCR1A &= ~((1U << COM1B1) | (1U << COM1B0));
         setOCR1B(txSettlingDelayEnd);
         TIM1state = TIM1mode::inTXpulseSettlingDelay;
         break;
       case TIM1mode::inTXpulseSettlingDelay:
         turn_on_builtin_led();

         setOCR1B(txSetupStart);
         TIM1state = TIM1mode::inRXcapture;
         //enable capture irq
         TIMSK1 |= (1U << ICIE1 );
         //clear flag
         TIFR1  = (1<< ICF1);
         break;
       case TIM1mode::inRXcapture:
         //disable capture irq
         TIMSK1 &= ~(1U << ICIE1 );
         //clear flag
         TIFR1  = (1<< ICF1);
         turn_off_builtin_led();
   
         txPulseSetup();
         break;
       default:
         // shouldnt get here!
         break; 
   }
}

namespace {
   volatile uint16_t capture_value = 0U;
   volatile bool new_capture = false;
}

/** 
   receive pulse capture from comparator
**/
ISR (TIMER1_CAPT_vect)
{
  /**
   read the ICR1 value and store in array
   for processing by main
   e.g NS, SN, EW, WE
   and
   **/
   capture_value = ICR1;
   //disable capture irq
   TIMSK1 &= ~(1U << ICIE1 );
            //clear flag
   TIFR1  = (1<< ICF1);
   
   new_capture = true;
}

void setup()
{
   Serial.begin(115200);
   builtin_led_setup();
   turn_off_builtin_led();
   // set up 
   txPulseInitialSetup();

   Serial.println("ultra starting");
   
}

void loop()
{
   uint16_t capt;
   cli();
      bool const got = new_capture;
      if (got){
         capt = capture_value;
         new_capture = false;
      }
   sei();
   if( got){
      Serial.print("Got ");
      Serial.print(capt);
      Serial.println();
   }
}