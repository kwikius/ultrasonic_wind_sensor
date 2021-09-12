
#include <Arduino.h>
#include <quan.h>
#include <quan/time.hpp>
#include <quan/frequency.hpp>

void set_ultrasound_address(uint8_t addr);

namespace {

   QUAN_QUANTITY_LITERAL(frequency, MHz)
   QUAN_QUANTITY_LITERAL(frequency, kHz)
   QUAN_QUANTITY_LITERAL(time, us)
   QUAN_QUANTITY_LITERAL(time, ms)

#if defined F_CPU
 #if F_CPU == 16000000
   quan::frequency::MHz constexpr systemClockFrequency = 16_MHz;
 #else
   #error unknown cpu frequency
 #endif
#else
 #error F_CPU undefined
#endif

   // transducer addresses
   constexpr uint8_t north_transducer = 3U;
   constexpr uint8_t south_transducer = 0U;
   constexpr uint8_t east_transducer  = 1U;
   constexpr uint8_t west_transducer =  2U;

   // ultrasound_flight id
   constexpr uint8_t flight_north_south = 0U;
   constexpr uint8_t flight_south_north = 1U;
   constexpr uint8_t flight_east_west = 2U;
   constexpr uint8_t flight_west_east = 3U;

   // the transmit address for the current flight
   uint8_t constexpr flight_transmit_address [] = {
      north_transducer,
      south_transducer,
      east_transducer,
      west_transducer
   };

   // the receive address for the current flight
   uint8_t constexpr flight_receive_address [] = {
      south_transducer,
      north_transducer,
      west_transducer,
      east_transducer
   };

   // id of the current flight
   // rotates fwd by 1 in txPulseSetup() so start at last to get first
   volatile uint8_t current_flight = flight_east_west;
   // raw flight capture values indexed by current flight
   volatile uint16_t ll_flight_capture_values[4] = {0U};

  // user layer values
   volatile uint16_t flight_capture_values[4] = {0U};
   volatile bool new_capture = false;

} // ~ namespace

   // set up transmit address for current flight
void set_transmit_address()
{
   set_ultrasound_address(flight_transmit_address[current_flight]);
}

// set up receive address for current flight
void set_receive_address(){
   set_ultrasound_address(flight_receive_address[current_flight]);
}

// transfer prev values to user buffer if last was taken
void start_next_flight()
{
   current_flight = (current_flight + 1U ) % 4U;

   if ( current_flight == 0U){
      if ( new_capture == false){
         for( uint8_t i  = 0; i < 4; ++i){
            flight_capture_values[i] = ll_flight_capture_values[i];
         }
         new_capture = true;
      }
   }
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
   ll_flight_capture_values[current_flight] = ICR1;
   //disable capture irq
   TIMSK1 &= ~(1U << ICIE1 );

   //new_capture = true;
   TIFR1  = (1<< ICF1);
}

namespace {

   quan::time::us convert_capture(uint16_t capture_value)
   {
      return capture_value / systemClockFrequency;
   }
}


/*
  returns true if new capture was put in result. If true also allows new capture
  else false and no change
  if capture timeout will return true and 0 in result
*/
bool get_ultrasound_capture(quan::time::us (& result)[4])
{
   if ( new_capture){
      for ( uint8_t i = 0; i < 4 ; ++i){
         result[i] = convert_capture(flight_capture_values[i]);
      }
      new_capture = false;
      return true;
   }else {
      return false;
   } 
}

#if 0
bool get_ultrasound_capture(quan::time::us & result)
{
    
    cli();
    bool const have_capture = new_capture;
    new_capture = false;
    uint16_t const new_capture_value = capture_value;
    sei();
    if ( have_capture){
       quan::time::us const t = new_capture_value / systemClockFrequency;
       result = t;
    }
    return have_capture;
}
#endif