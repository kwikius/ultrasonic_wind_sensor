
#include <Arduino.h>

void set_ultrasound_address(uint8_t addr);

namespace {

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
   volatile uint8_t current_flight = flight_north_south;
   // raw flight capture values indexed by current flight
   // as they are updated in irq
   volatile uint16_t ll_flight_capture_values[4] = {0U};

  // buffered user layer raw values
   // valid when new_capture is true
   volatile uint16_t flight_capture_values[4] = {0U};
   volatile bool new_capture = false;

} // ~ namespace


// get ref to latest captures or null if not yet available
volatile uint16_t * get_captures()
{
   bool new_capt = new_capture;
   new_capture = false;
   if ( new_capt){
     return flight_capture_values;
   }else {
      return nullptr;
   }
}
namespace {
      // set up transmit address for current flight
   void set_transmit_address()
   {
      set_ultrasound_address(flight_transmit_address[current_flight]);
   }
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
   set_transmit_address();
}

namespace {
  volatile uint16_t ll_capture_value = 0U;
  volatile bool ll_new_capture_value = false;
}

/** 
   receive pulse capture
**/
ISR (TIMER1_CAPT_vect)
{
   ll_capture_value = ICR1;
   TIMSK1 &= ~(1U << ICIE1 );
   ll_new_capture_value = true;
}

namespace {
   /*
    call from irq only
   */
   bool ll_get_capture(volatile uint16_t & result)
   {
      bool const new_capt = ll_new_capture_value;
      ll_new_capture_value = false;
      if ( new_capt){
         result = ll_capture_value;
      }
      return new_capt;
   }
}
/**
* 
**/
void validate_capture()
{
    volatile uint16_t & target = ll_flight_capture_values[current_flight];
    if (!ll_get_capture(target)){
       target = 0U;
    }
}




