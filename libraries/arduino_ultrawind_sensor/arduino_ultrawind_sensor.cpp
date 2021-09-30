
/**
 * @brief quan.h is required for arduino framework to
 *        pick up the quan library in the ../../libraries folder
*/
#include <quan.h>
#include <quan/time.hpp>
#include <quan/frequency.hpp>
#include <builtin_led.h>
#include "flight_sequence.h"

// for showing output at human visible speed
//#define UWS_SLOW_MOTION_DEBUG
// TODO try using atmega328 comparator. Unfortunately it doesnt have an output
//#define UWS_EXTERNAL_COMPARATOR

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
   quan::time::us constexpr txSettlingDelay = 1_ms;

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
   *  @brief time required to start setting up for next tx Pulse
   **/
   quan::time::us constexpr txSetupTime = txSetupCycles / systemClockFrequency;

  /**
   * @brief available time from start of ultrasound tx pulse to detect pulse
   * at receiver transducer
   **/ 
   quan::time::us constexpr rxCaptureTime = tim1Period - txSetupTime;

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
    * @brief set up 4052 Addresses for driving the tx pulse and listening to rx pulses
   **/
   void setupTransducerAddressPorts()
   {
      PORTC &= ~(0b11 << 2U);
      DDRC |= (0b11 << 2U);
   }
}

/**
* @brief set ultrasound address to addr
  @param addr
**/
void set_ultrasound_address(uint8_t addr)
{
   PORTC = (PORTC & ~(0b11 << 2U)) | ((addr & 0b11) << 2U);
}

namespace {

   /**
    *  @brief stop timer1
   **/
   void disableTIM1()
   {
       //errata : for stopping clock Writing TCCR1A WGM bits seems required else hangs up
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

#if 0
   /**
    * @brief set OCR1B to give a compare event at t
    * called in irq
    * @note make sure that t is constexpr
   **/
   inline void setOCR1B(quan::time::us const & t)
   {
      OCR1B = systemClockFrequency * t - 1U;
   }

   inline void setOCR1A(quan::time::us const & t)
   {
      OCR1A = systemClockFrequency * t - 1U;
   }
#else
   // verify its constexpr
   #define setOCR1B(Time) \
      {\
      constexpr auto name = systemClockFrequency * ( Time )-1U; \
      OCR1B = name; \
      }

      #define setOCR1A(Time) \
      {\
      constexpr auto name = systemClockFrequency * ( Time )-1U; \
      OCR1A = name; \
      }
#endif
   
   /**
   * @brief usually called in interrupt.
   * Setup for next tx pulse
   **/
   void txPulseSetup()
   {
      start_next_flight(); 
      
      // N.B OCR1A, OCR1B double buffered, so will load at TOV
      setOCR1A(txSettlingDelay);
      setOCR1B(transducerCycleTime / 2.f);
      // set OC1B to set at BOTTOM, clear on pulseCount match
      TCCR1A |= (0b10 << COM1B0);
   
      // pulse starts at TIM1 overflow
      // enable the Pulse start interrrupt
      TIMSK1 = (0b1 << TOIE1);
      // clear irq flags (N.B. write a 1 to bit to clear flag)
      TIFR1 = 0b00100111;
      TIM1state = TIM1mode::preTXpulse;
   }

}// namespace

void txPulseInitialSetup()
{
   cli();
   {
      disableTIM1();

         setupTransducerAddressPorts();
      
         //set up  OC1B(PB2) as low output 
         PORTB &= ~(1U << 2U);
         DDRB  |= (1U << 2U);
        
         // TIM will overflow ASAP and start pulse
         TCNT1 = 0xFFFE;
         
         //set fast pwm mode 15 OCR1A is TOP, OCR1B is comp
         TCCR1A |= (0b11 << 0U);
         TCCR1B |= (0b11 << 3U);

         txPulseSetup();
        
       enableTIM1();  // to start
   }
   sei();
}

ISR (TIMER1_OVF_vect)
{
   switch (TIM1state){
      case TIM1mode::preTXpulse:
         // tx pulse has just gone high
         TIM1state = TIM1mode::inTXpulse;
         TIMSK1 = (1U << OCIE1B);
         TIFR1 = 0b00100111;
         break;
      case TIM1mode::inTXpulseSettlingDelay:
         // start of rx capture 
         TIM1state = TIM1mode::inRXcapture;
         // enable Capture irq and end of capture irq
         TIMSK1 = (1U << ICIE1 ) | (1U << OCIE1B) ;
         //clear  irq flags
         TIFR1 = 0b00100111;
         break;
      default:
         break;
   }
}

ISR (TIMER1_COMPB_vect)
{
   switch(TIM1state){
      case TIM1mode::inTXpulse:
         // end of tx pulse
         // disable pulse output
         TCCR1A &= ~((1U << COM1B1) | (1U << COM1B0));
         set_receive_address();
         // N.B OCR1A, OCR1B double buffered, so will load at TOV
         setOCR1B(rxCaptureTime);
         setOCR1A(rxCaptureTime + txSetupTime);
         TIM1state = TIM1mode::inTXpulseSettlingDelay;
         TIMSK1 = (1U << TOIE1);
         TIFR1 = 0b00100111;
         break;
      case TIM1mode::inRXcapture:
         // end of rxCapture
         // get capture if any
         validate_capture();
         txPulseSetup();
         break;
      default:
         break;
   }

}

namespace {

   quan::time::us convert_capture(uint16_t capture_value)
   {
      return capture_value / systemClockFrequency + txSettlingDelay;
   }
}

bool get_ultrasound_capture(quan::time::us (& result)[4])
{
   volatile uint16_t const * const captures = get_captures();
   if ( captures != nullptr){
      for ( uint32_t i = 0; i < 4 ;++i){
         result[i] = convert_capture(captures[i]);
      }
      clear_capture();
      return true;
   }else{
      return false;
   }
}
