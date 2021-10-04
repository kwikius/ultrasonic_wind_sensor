
#include <Arduino.h>
#include <builtin_led.h>

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

   /**
    * Use 2 data buffers
    * So irq can write new data while main can read the last data
   **/
   volatile uint16_t flight_capturesJ [4] = {0U};
   volatile uint16_t flight_capturesK [4] = {0U};

   volatile uint16_t * write_flight_captures = flight_capturesJ;
   volatile uint16_t * read_flight_captures = flight_capturesK;

   void swap_flight_capture_buffers()
   {
      volatile uint16_t * const temp = read_flight_captures;
      read_flight_captures = write_flight_captures;
      write_flight_captures = temp;
   }
   /** low level flag
    * set by irq when new data is ready in read buffer
    * cleared by main when read buffer has been read
    * N.B assume read atomic write atomic
    **/
   volatile bool new_capture = false;

} // ~ namespace

/** @brief get pointer to latest captures.
 @ Called by main
 * @return a pointer to the array of new capture values 
 *  or nullptr if not yet available
**/ 
volatile uint16_t const * get_captures()
{
   if (new_capture){
      return read_flight_captures;
   }else {
      return nullptr;
   }
}

/**
  @brief called by main to signal it has read the contents of current read buffer 
  and no longer needs it.
  conditions
  get_captures has returned the current read buffer rather than nullptr
  N.B that get_captures is called only in get_ultrasound_capture
**/
void clear_capture()
{
   new_capture = false;
}

namespace {
      // set up transmit address for current flight. Used by irq
   void set_transmit_address()
   {
      set_ultrasound_address(flight_transmit_address[current_flight]);
   }
}

// set up receive address for current flight. Used by irq
void set_receive_address(){
   set_ultrasound_address(flight_receive_address[current_flight]);
}

/**
* Start next flight. used by irq
* If end of sequence, start next sequence
* if old data was taken, swap read and write buffers, 
* else just overwrite last write data buffer
* so no swapping of buffers to prevent corrupting read buffer
* as main is possibly reading it 
**/
void start_next_flight()
{
   current_flight = (current_flight + 1U ) % 4U;
   if ( (current_flight == 0U) && (new_capture == false)){
      swap_flight_capture_buffers();
      new_capture = true;
   }
   set_transmit_address();
}

namespace {
  volatile uint16_t ll_capture_value = 0U;
  volatile bool ll_new_capture_value = false;
}

/** 
   receive pulse capture interrupt
   Read the capture data register
   disable capture interrupt
   and signal a new capture 
**/

namespace {
  volatile int irqcount = 0;
}
ISR (TIMER1_CAPT_vect)
{
   ll_capture_value = ICR1;
   TIMSK1 &= ~(1U << ICIE1 );
   ll_new_capture_value = true;
   if (++irqcount == 1000){
    complement_builtin_led();
    irqcount = 0;
   }
}

namespace {
   /*
    called from irq only
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
* called from irq only. Either a valid capture s put into current write data slot
or not valid, then set current write data slot value to 0
**/
void validate_capture()
{
    volatile uint16_t & target = write_flight_captures[current_flight];
    if (!ll_get_capture(target)){
       target = 0U;
    }
}
