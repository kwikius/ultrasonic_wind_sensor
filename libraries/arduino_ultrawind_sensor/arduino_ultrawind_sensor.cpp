
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

//######## Config ############
// define to use external comparator rather than internal atmega328 one
//#define UWS_EXTERNAL_COMPARATOR

#if !defined UWS_EXTERNAL_COMPARATOR
// internal comparator using adc mux to change pins
  #define MUX_COMP_INPUTS
#endif

#if defined (MUX_COMP_INPUTS) && defined(UWS_EXTERNAL_COMPARATOR)
#error incompatible options
#endif
//############## Config ##############

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
   QUAN_QUANTITY_LITERAL(time, ns)
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
    RxSignal AIN0     PD6      10       D6   positive input (Output from bandpass filter)
    if MUX_COMP_INPUTS{
     ZeroCrossingIn   ADC0  PC0    23  A0       neg input Filter Gnd
     EnvelopeDetectIn   AIN1  AIN1     PD7      11       D7 neg input envelope
    }else{
      FilterGnd     AIN1     PD7      11       D7    Filter Gnd
      CmpOut                 PD5       9       D5    software comparator output. N.B there is no actual hardware comp output
    }
 
   }
   
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
/**
  * @brief holds the current state of the TIM1 state machine
**/
   volatile auto TIM1state = TIM1mode::undefined;

   void setOCR1B(quan::time::us const & t)
   {
      OCR1B = systemClockFrequency * t -1U;
   }

   void setOCR1A(quan::time::us const & t)
   {
      OCR1A = systemClockFrequency * t -1U;
   }

   /**
   * @brief Setup for next tx pulse
   * usually called in interrupt.
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
      // enable the on pulse start interrupt
      TIMSK1 = (0b1 << TOIE1);
      // clear irq flags (N.B. write a 1 to bit to clear flag)
      TIFR1 = 0b00100111;
      TIM1state = TIM1mode::preTXpulse;
   }

#if !defined (UWS_EXTERNAL_COMPARATOR)

#if !defined (MUX_COMP_INPUTS)
/**
 * @brief As atmega328p has no comp output so
 * we have to do it in software
**/
  inline void clearSoftCompOutput() 
  {
    PORTD &= ~(0b1 << 5U);
  }

  inline void setSoftCompOutput() 
  {
    PORTD |= (0b1 << 5U);
  }
#endif
  /**
    called at setup
    set up comparator
    Use default negative pin
  **/
  void comparatorSetup()
  {
#if !defined (MUX_COMP_INPUTS)
     // make the software output PD5 low output
     PORTD &= ~(0b1 << 5U);
     DDRD  |= (0b1 << 5U);
#endif

#if defined (MUX_COMP_INPUTS)
     // ADC must be off . N.B could do this in comparator irq
     // so ADC could be available except during zero crossing?
     ADCSRA &= ~(0b1 << ADEN);
     // Sets AIN1 Envelope detect as comparator negative input
     // no multiplexing of comp negative input to adc pins
     ADCSRB &= ~(0b1 << ACME);
     // set the adc address of bandpass filter gnd on ADC multiplexer
     // to prepare mux to switch to it by setting ADCSRB.ACME
     constexpr uint8_t  bandpass_filter_gnd_addr = 0U; //set pin to ADC0
     ADMUX = (ADMUX & ~(0b1111 << MUX0)) | (bandpass_filter_gnd_addr << MUX0); 
#endif
     // disable digital input on AIN1/0
     DIDR1 |= (0b11 << AIN0D);
#if defined (MUX_COMP_INPUTS)
     // disable digital input on ADC0
     DIDR0 |= (0b1 << ADC0D);
#endif
     // disable comparator interrupt for now
     ACSR &= ~(0b1 << ACIE) ;
     // set comparator to interrupt on rising edge "envelope detect"
     ACSR |= (0b11 << ACIS0);
     // set the comparator to trigger a capture on timr1 ( on falling edge)
     ACSR |= (0b1 << ACIC);
     //clear Timer1 irq flags after switching timr1 capture source as per manual
     TIFR1 = (0b1 << ICF1);
     
  }
#endif

}// namespace

void txPulseInitialSetup()
{
   cli();
   {
      disableTIM1();

         setupTransducerAddressPorts();
      
         //set up  OC1B(PB2) as low output 
         PORTB &= ~(0b1 << 2U);
         DDRB  |= (0b1 << 2U);
        
         // TIM will overflow ASAP and start pulse
         TCNT1 = 0xFFFE;
         
         //set fast pwm mode 15 OCR1A is TOP, OCR1B is comp
         TCCR1A |= (0b11 << 0U);
         TCCR1B |= (0b11 << 3U);

         txPulseSetup();
#if !defined (UWS_EXTERNAL_COMPARATOR)
        comparatorSetup();
#endif
      enableTIM1();  // to start
   }
   sei();
}

/**
 * Timer1 timer overflow interrupt action depends on Tim1 state machine
**/

ISR (TIMER1_OVF_vect)
{
   switch (TIM1state){
      case TIM1mode::preTXpulse: // tx pulse has just gone high
         TIM1state = TIM1mode::inTXpulse;
         // now want to enable the compare irq when TX pulse goes low
         TIMSK1 = (1U << OCIE1B);
         // and clear all flags
         TIFR1 = 0b00100111;
         break;
      case TIM1mode::inTXpulseSettlingDelay: // start of rx capture period
         TIM1state = TIM1mode::inRXcapture;
#if !defined (UWS_EXTERNAL_COMPARATOR)
     // using internal comparator
#if defined (MUX_COMP_INPUTS)
   // using adc multiplexer
         // TODO : here turn off A2D if necessary
         // Set AIN1 Envelope detect as comparator negative input 
         ADCSRB &= ~(0b1 << ACME);
#endif
         // enable end of capture irq in case there is
         // no receive pulse detected in the allocated time
         TIMSK1 = (1U << OCIE1B) ;
         //clear  Tim1 irq flags
         TIFR1 = 0b00100111;
         // enable comparator interrupt on rising edge envelope detect
         ACSR |= (0b1 << ACIE);
         // clear junk from comparator interrupt flag
         ACSR  &= ~(0b1 << ACI) ;
#else  
  // external comparator
         // enable Capture irq and end of capture irq
         TIMSK1 = (1U << ICIE1 ) | (1U << OCIE1B) ;         //clear  irq flags
         TIFR1 = 0b00100111;
#endif
         break;
      default:
         break;
   }
}

#if !defined (UWS_EXTERNAL_COMPARATOR)
/**
 Analog comparator rising edge envelope detected interrupt.
  if not using multiplex option, use comparator interrupt 
  to flip the software impl of comparator output high ready
  to detect zero crossing by falling edge.
  If using multiplex option, switch the negative comparator input
  to the 40 kHz bandpass filter ground, ready to detect zero crossing
  by falling edge

The comparator characteristics are given in the atmel/avr datasheet
8271G–AVR–02/2013 
section 29.2  table 29-1 "Common DC characteristics ... V CC = 1.8V to 5.5V"

comparator electrical characteristics
input offset voltage @ 5V 
(conditions 0.4V < Vin <Vcc-0.5V)
   typical 10 mV, max 40 mV
input leakage current           
   min -50 nA max 50 nA
propogation delay
   500 ns

From envelope detect there is min of 1/4 of a cycle of 40 kHz receiver 
waveform before next zero crossing. This means a minimum of 6.25 usec 
between envelope-detect ( not including mcus propogations delays)
and the zero-crossing-capture. The mode at time of interrupt is 
listening for the receiver pulse, so there are no other interrupts to 
get in the way ( except the end of rx capture interrupt, which, if it 
occurs first, indicates there was no pulse received in the time, so not 
a problem).

interrupt response time
from 8271G–AVR–02/2013  7.7.1

longest instruction length is 4 cycles ( e.g CALL, RET) which will finish 
before irq starts.
4 cycles to enter irq.
3 cycles for jump vector.
Total then 11 cycles so 0.75 usec < 1.0 usec.

Allowing 1 usec for the output to settle after it has been changed, still 
allows 4.5 usec before the zero crossing pulse, so should be ok!
**/

namespace {
  int irqcount = 0;
}
ISR (ANALOG_COMP_vect)
{

#if !defined (MUX_COMP_INPUTS)
  // Set the positive feedback output pin high ASAP.
  // Note there is a possibility the output will have gone 
  // low and will pulse high again so flags may be set.
   setSoftCompOutput();  // < 1 usec from envelope detect on comp +ve input
#else

   // Set ADC0 (filter GND) as comparator negative input
   // by enabling adc multiplexer comparator mode
   ADCSRB |= (0b1 << ACME);

#endif
   // disable comparator interrupt,
   ACSR &= ~(0b1 << ACIE) ; // assume CBI  2 cycles
   // enable the TIM1 capture interrupt, to capture  
   // receiver 40 kHz waveform zero crossing very soon!
 //  TIMSK1 = (0b1 << ICIE1 );  // assume LDI 1 cycle
   TIMSK1 = (1U << ICIE1 ) | (1U << OCIE1B) ;

   // Delay for the positive feedback output to change. 
   // Allow 1 usec according  to spec cited above 
   // (allows 0.5 usec extra for if the resistor network 
   // has high values of resistance and capacitance).
#if defined (MUX_COMP_INPUTS)
   // TBH maybe this could be less in multiplex comparator mode?
   // No outputs need switching, just internal multiplexer
#endif
   constexpr auto delay_time = 1.0_us;
   // cycles already used for the above instructions
#if 0
   // for led turn on
   constexpr auto used_cycles = 5U;
 // if ( ++irqcount == 1000){
   complement_builtin_led();
 //  irqcount = 0;
//  }
#else
   constexpr auto used_cycles = 3U;
#endif

   __builtin_avr_delay_cycles(delay_time * systemClockFrequency - used_cycles); 

  // Assume the comparator output has settled ( high)  after delay,
//###########################################################
// TODO we could check the ACSR.ACO bit to see the 
 // comparator output state and set error flag if not high here?
//############################################################
   // so clear the TIM1 input capture flag, which may have 
   // pulsed before the positive feedback took effect.
   TIFR1 = (0b1 << ICF1); // < 2 usec from envelope detect on comp +ve input
}
#endif

ISR (TIMER1_COMPB_vect)
{
   switch(TIM1state){
      case TIM1mode::inTXpulse:// end of tx pulse
         // disable pulse output
         TCCR1A &= ~((1U << COM1B1) | (1U << COM1B0));
         set_receive_address();
         // N.B OCR1A, OCR1B double buffered, so will load at TOV
         setOCR1B(rxCaptureTime);
         setOCR1A(rxCaptureTime + txSetupTime);
         TIM1state = TIM1mode::inTXpulseSettlingDelay;
         // enable TIM1 timer overflow interrupt
         TIMSK1 = (0b1 << TOIE1);
         // clear timr1 interrupt flags
         TIFR1 = 0b00100111;
         break;
      case TIM1mode::inRXcapture: // end of rxCapture period
         // get capture if any...
         validate_capture();

         // set up for next cycle
         #if !defined (UWS_EXTERNAL_COMPARATOR) && !defined (MUX_COMP_INPUTS)
            // end of cycle, so reset software driven comparator output low
            clearSoftCompOutput();
         #endif
         txPulseSetup();
         break;
      default:
         // shouldnt get here!
         break;
   }
}

namespace {

   quan::time::us convert_capture(uint16_t capture_value)
   {
      return capture_value / systemClockFrequency + txSettlingDelay;
   }
}

/**
* @brief Called by main to poll if there is new data
* If returns true then new data has been put in result and 
* another call will not return true till next data is available
* If returns false, no side effects and result is unaffected
**/
bool get_ultrasound_capture(quan::time::us (& result)[4])
{
   volatile uint16_t const * const captures = get_captures();
   if ( captures != nullptr){
      // new data...
      for ( uint32_t i = 0; i < 4 ;++i){
         result[i] = convert_capture(captures[i]);
      }
      // signal to the irq that we are done with the read buf data
      clear_capture(); 
      return true;
   }else{
      // no new data
      return false;
   }
}
