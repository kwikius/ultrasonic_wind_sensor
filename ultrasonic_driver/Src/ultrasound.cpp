

/*
  set up to periodically send an ultrasound pulse stream

  TODO one shot or continuous
  
  params 
    ultrasound frequency : say 40 kHz
    length of stream      number of pulses : say 10
    record of time when sent in a timer to pin for stop
    
    max timing dist of sensors and wind speed
    
    max clock frequency 16 or 32 bit timer
    

  Use TIM2 for counting the length of the pulse. 32 bit counter
  so can count longer pulses
  Use TIM1 as PWM. Use TIM1.RCR reg to output a specific number of pulses.
  To sync start of count with pulse train, make TIM1 a slave of TIM2
  (use TIM2 enable as trigger output)
  
  PA8 TIM1_CH1   AF1
*/

#include "resource.hpp"
#include <quan/stm32/get_raw_timer_frequency.hpp>
#include <quan/stm32/rcc.hpp>
#include <quan/stm32/tim/temp_reg.hpp>
#include <quan/stm32/tim/detail/get_irq_number.hpp>
#include <quan/stm32/irq_handler.hpp>
#include <quan/time.hpp>
#include <quan/frequency.hpp>
#include <quan/fixed_quantity/literal.hpp>
#include <quan/length.hpp>
#include <quan/velocity.hpp>

namespace {

   // transducer addresses
   constexpr uint32_t north_transducer = 3U;
   constexpr uint32_t south_transducer = 0U;
   constexpr uint32_t east_transducer  = 1U;
   constexpr uint32_t west_transducer =  2U;

   // ultrasound_flight id
   constexpr uint32_t flight_north_south = 0U;
   constexpr uint32_t flight_south_north = 1U;
   constexpr uint32_t flight_east_west = 2U;
   constexpr uint32_t flight_west_east = 3U;

   void set_ultrasound_address(uint8_t addr)
   {
       uint32_t const new_odr = 
          (quan::stm32::gpiob::get()->odr.get() & ~(0b11 << 6U)) | 
            ( (addr % 4U) << 6U) ;

       quan::stm32::gpiob::get()->odr.set( new_odr );
   }

   // the transmit address for the current flight
   uint32_t constexpr flight_transmit_address [] = {
      north_transducer,
      south_transducer,
      east_transducer,
      west_transducer
   };

   // the receive address for the current flight
   uint32_t constexpr flight_receive_address [] = {
      south_transducer,
      north_transducer,
      west_transducer,
      east_transducer
   };

   // id of the current flight
   uint32_t current_flight = flight_north_south;

   // set up transmit address for current flight
   void set_transmit_address()
   {
      set_ultrasound_address(flight_transmit_address[current_flight]);
   }

   // set up treceive address for current flight
   void set_receive_address(){
      set_ultrasound_address(flight_receive_address[current_flight]);
   }

   // raw flight capture values indexed by current flight
   uint32_t ll_flight_capture_values[4] = {0U};

   // address 0 to 3
   // address 0 = PB4
   // address1 = PB5
   // TODO lock port with mutex before access if anyone else may be using it

   QUAN_QUANTITY_LITERAL(frequency,kHz)
   QUAN_QUANTITY_LITERAL(frequency,Hz)
   QUAN_QUANTITY_LITERAL(time,us)
   QUAN_QUANTITY_LITERAL(time,ms)
   QUAN_QUANTITY_LITERAL(velocity,m_per_s)

/*
TODO

    Check that combo is valid?
    One shot?
*/
  // quan::velocity::m_per_s sound_velocity = 344.7_m_per_s;
   quan::frequency_<int32_t>::Hz ultrasound_frequency = 40_kHz;
   // N.B that 4 samples are used for a full measurement
   // tof_update-frequency is for one sample

   quan::frequency_<int32_t>::Hz tof_update_frequency = 50_Hz;
   int num_ultrasound_pulses = 1;

/*
   TODO make this init deinit etc
   TODO check sanity of params
*/
   void setup_tof_timer()
   {
      // enable tof_timer clock
      quan::stm32::module_enable<tof_timer>();
      
      // set up tof_timer for counting
      auto constexpr clk_freq = 
         quan::frequency_<int32_t>::Hz{quan::stm32::get_raw_timer_frequency<tof_timer>()};

      //N.B use prescaler to slow down timer for testing
      // tim2 ( tof_timer) sets the frequency of ultraound sampling
      tof_timer::get()->psc = 0; // 
      tof_timer::get()->arr = clk_freq / tof_update_frequency - 1U; 
      
      // set up tof_timer so that starting gives immediate overflow
      // in case where we want continuous pulses
      // since overflow triggers TRGO
      tof_timer::get()->cnt = tof_timer::get()->arr;

      // capture PA0 TIM2_CH1
      // N.B. TODO:can filter the input so that is stable for some time before capture
      // Filter can use clock or clock divided DTS available in CR1.CKD register
       {
         quan::stm32::tim::ccmr1_t ccmr1 = tof_timer::get()->ccmr1.get();
         ccmr1.cc1s = 0b01; // capture in CCR1
          
         tof_timer::get()->ccmr1.set(ccmr1.value);       
      }
      // select falling edge ccer.cc1p = 1
       
      {
         quan::stm32::tim::ccer_t ccer = 0;
         //Select falling edge is 01 on CC1NP CC1P bits
         ccer.cc1p = 1; 
         ccer.cc1np = 0;    
         ccer.cc1e = 0;    // Dont enable capture yet
         tof_timer::get()->ccer.set(ccer.value);
      }

      // time in cycle when we have finished the output pulses
      quan::time::us const pulse_end_time 
         = static_cast<double>(num_ultrasound_pulses) / ultrasound_frequency;

      // how long to wait for filter to settle after pulse before setting up receive internals
      // The
      quan::time::us constexpr settling_delay = 1_ms;

      auto const receive_init_time = pulse_end_time + settling_delay;

      // N.B: min dist that can be usefully measured = sound velocity * receive_init_time
      uint32_t const receive_init_count = receive_init_time * clk_freq;
      tof_timer::get()->ccr3 = receive_init_count;

      // set up compare interrupt to do housekeeping before pulse output
      // use ch3 as compare
      // want to give a reasonable time before next ultrasound cycle
       // which is  T * clk_freq count
      quan::time::us constexpr pre_irq_time = 100_us;
      // n.b longest dist that can be measured = sound_velocity * (cycle_time - pre_irq_time)
      uint32_t constexpr pre_irq_count = pre_irq_time * clk_freq;
       //sanity check
      static_assert(pre_irq_count == 8000,"");
       
      tof_timer::get()->ccr4 = tof_timer::get()->arr - pre_irq_count;
      //set ccr3,cc4 as compare
      {
         quan::stm32::tim::ccmr2_t ccmr2 = tof_timer::get()->ccmr2.get();

         ccmr2.cc3s = 0b00;
         ccmr2.oc3m = 0b0001; // OC3REF active high on match
         ccmr2.cc4s = 0b00;
         ccmr2.oc4m = 0b0001; // OC4REF active high on match

         tof_timer::get()->ccmr2.set(ccmr2.value);       
      }

      static constexpr IRQn_Type irq_number = 
          quan::stm32::tim::detail::get_irq_number<tof_timer>::value; 

      // set enable as tof_timer trigger output
      // or set update as trigger out for one shot
      {
         quan::stm32::tim::cr2_t cr2 = tof_timer::get()->cr2.get();

         //cr2.mms = 0b001; // Enabling the timer causes TRGO
         cr2.mms = 0b010; // Update event causes TRG0

         tof_timer::get()->cr2.set(cr2.value);
      }

      NVIC_SetPriority(irq_number,14); // low priority 
      NVIC_EnableIRQ(irq_number);

      tof_timer::get()->dier.bb_setbit<3>(); // (CC3IE)
      tof_timer::get()->dier.bb_setbit<4>(); // (CC4IE)
      // update shadow regs
      tof_timer::get()->egr = (1<<0); // (UG)
     // tof_timer::get()->sr = 0U;
      
   }
     
   void setup_ultrasound_pins()
   {
      //enable clock for pin
      quan::stm32::module_enable<pulse_stream_out_pin::port_type>();  
      quan::stm32::apply<
         pulse_stream_out_pin
         ,quan::stm32::gpio::mode::af1  // PA8 TIM1_CH1
         ,quan::stm32::gpio::pupd::none
         ,quan::stm32::gpio::ospeed::slow
      >();

      quan::stm32::module_enable<envelope_detect_pin::port_type>(); 
      quan::stm32::module_enable<zero_crossing_pin::port_type>();

      // start address = 00
      quan::stm32::module_enable<ultrasound_address0_pin::port_type>();
      quan::stm32::apply<
         ultrasound_address0_pin
         ,quan::stm32::gpio::mode::output  
         ,quan::stm32::gpio::pupd::none
         ,quan::stm32::gpio::otype::open_drain
         ,quan::stm32::gpio::ospeed::slow
         ,quan::stm32::gpio::ostate::low
      >();

      quan::stm32::module_enable<ultrasound_address1_pin::port_type>();
      quan::stm32::apply<
         ultrasound_address1_pin
         ,quan::stm32::gpio::mode::output  
         ,quan::stm32::gpio::pupd::none
         ,quan::stm32::gpio::otype::open_drain
         ,quan::stm32::gpio::ospeed::slow
         ,quan::stm32::gpio::ostate::low
      >();

     //set up envelope_detect_pin PA0 (A0) TIM2_CH1 AF1
     quan::stm32::apply<
         envelope_detect_pin
         ,quan::stm32::gpio::mode::af1  
         ,quan::stm32::gpio::pupd::none
         ,quan::stm32::gpio::ospeed::fast
      >();
    
     // TODO set up zero crossing capture

   }
     
     void setup_pulse_timer()
     {
        setup_tof_timer();
        quan::stm32::module_enable<pulse_timer>();
        
        setup_ultrasound_pins();
        set_ultrasound_address(0U);
        
        auto constexpr pulse_timer_clk_freq = 
           quan::frequency::Hz{quan::stm32::get_raw_timer_frequency<pulse_timer>()};

        // cr1 is all 0
        // cr2 is all 0
        // smcr currently 0 but will be used to sync start
        {
             quan::stm32::tim::smcr_t smcr = 0;

             // timer is started by TRGI
            // smcr.sms = 0b0110;
            // smcr.sms3 = 0;

             // timer is reset and started by TRGI
             smcr.sms = 0b000;
             smcr.sms3 = 1;

             smcr.ts = 0b001; // trigger is ITR1 which is TIM2.TRGO
             pulse_timer::get()->smcr.set(smcr.value);
        }
        // dier is all 0
        // sr is cleared just before start
        // egr is all 0
        // ccmr1 configure timer to pins
        // PA8 TIM1_CH1 as pwm output
        {
             // set ch[1] to PWM mode 1
             quan::stm32::tim::ccmr1_t ccmr1 = 0;
             ccmr1.cc1s = 0b00;  // output
             ccmr1.oc1m = 0b110; // PWM mode 1
             ccmr1.oc1ce = false;
             pulse_timer::get()->ccmr1.set(ccmr1.value);
        }
        // ccmr2 is all 0


        auto const count_value = pulse_timer_clk_freq / ultrasound_frequency; 
        //ccr2,ccr3,ccr4 = 0
        pulse_timer::get()->arr = count_value - 1U;
         // setting cnt value above ccr1 value so that output OCR_REF stays low until start
        pulse_timer::get()->cnt = count_value - 1U;
        //compare reg at half ar for 50% duty cycle
        pulse_timer::get()->ccr1 = count_value / 2U  - 1U;

        pulse_timer::get()->psc = 0U;

        // ccer need to be set up to output on ch1
        // N.B once cc1e is enable so output follows the count ccr1 logic
        // so make sure that ccr1 is set and  cnt is > ccr1 value
        // before enabling
        {
           quan::stm32::tim::ccer_t ccer = 0;
           ccer.cc1e = true; //enable ch1 compare output on pin
           ccer.cc1p = 0;    // active high
           pulse_timer::get()->ccer.set(ccer.value);
        }

       // rcr disables update events for given number of updates
        pulse_timer::get()->rcr = 0;
        // bdtr moe needs to be set else no output
        {
          quan::stm32::tim::bdtr_t bdtr = 0;
          bdtr.moe = true;
          bdtr.ossr = false;
          bdtr.ossi = false;
          pulse_timer::get()->bdtr.set(bdtr.value);
        }

        // dmar not used
        // or1 =0
        // ccmr3 not used
        // ccr5,ccr6 not used
        // or2,or3 not used

         // for initial testing
         // enable update irq
         static constexpr IRQn_Type irq_number = 
            quan::stm32::tim::detail::get_irq_number<pulse_timer>::value;

         NVIC_SetPriority(irq_number,14); // low priority 
         NVIC_EnableIRQ(irq_number);

         // enable cc irq
         NVIC_SetPriority(TIM1_CC_IRQn,14);
         NVIC_EnableIRQ(TIM1_CC_IRQn);

       //  pulse_timer::get()->dier.bb_setbit<0>(); // (UIE)
         pulse_timer::get()->dier.bb_setbit<1>(); // (CC1E)
         pulse_timer::get()->sr = 0U;
     }

  // user layer values
   uint32_t flight_capture_values[4] = {0U};
   bool new_capture = false;

   // transfer prev values to user buffer if last was taken
   void start_next_flight()
   {
      current_flight = (current_flight + 1U ) % 4U;

      if ( current_flight == 0U){
         if ( new_capture == false){
            for( uint32_t i  = 0; i < 4; ++i){
               flight_capture_values[i] = ll_flight_capture_values[i];
            }
            new_capture = true;
         }
      }
      
   }

   quan::time::us convert_capture(uint32_t capture_value)
   {
      auto constexpr timer_freq = 
         quan::frequency::Hz{quan::stm32::get_raw_timer_frequency<tof_timer>()};
      quan::time::us const flight_time = static_cast<float>(capture_value) / timer_freq;
      return flight_time ;
   }
}

/*
  returns true if new capture was put in result. If true also allows new capture
  else false and no change
  if capture timeout will return true and 0 in result
*/
bool get_envelope_capture(quan::time::us (& result)[4])
{
   if ( new_capture){
      for ( uint32_t i = 0; i < 4 ; ++i){
         result[i] = convert_capture(flight_capture_values[i]);
      }
      new_capture = false;
      return true;
   }else {
      return false;
   } 
}

extern "C" __attribute__ ((interrupt ("IRQ") ))
void TIM2_IRQHandler()
{
   //compare irq 
   // after fixed delay from end of pulse train
   // enable capture of receive pulse
   if ( tof_timer::get()->sr.bb_getbit<3>() ){ //(CC3IF)
      tof_timer::get()->sr.bb_clearbit<3>();
   // set capture of envelope detect
      tof_timer::get()->sr.bb_clearbit<1>(); // (CC1IF)
      tof_timer::get()->ccer.bb_setbit<0>() ;//(CC1E)
      // TODO check that capture flag isnt set
      // which would mean envelope_detect already gone low
      // so ignore this cycle. Or can just disallow if too short?

      tof_timer::get()->dier.bb_setbit<1>(); // (CC1IE)
   }

  // compare irq
    // effective end of measurement cycle
   // ignore any receiver pulse now - 
   // Time to set up for next cycle
   if ( tof_timer::get()->sr.bb_getbit<4>() ){ //CC4IF

      tof_timer::get()->sr.bb_clearbit<4>(); // (CC4IF)
      tof_timer::get()->sr.bb_clearbit<1>(); // (CC1IF)
      tof_timer::get()->ccer.bb_clearbit<0>() ;//(CC1E)
      tof_timer::get()->dier.bb_clearbit<1>(); // (CC1IE)

      start_next_flight();  
      set_transmit_address();

   }

   //capture of envelope detect
   if ( tof_timer::get()->sr.bb_getbit<1>() ){ //(CC1IF)
      tof_timer::get()->sr.bb_clearbit<1>(); // (CC1IF)
      tof_timer::get()->ccer.bb_clearbit<0>() ;//(CC1E)
      tof_timer::get()->dier.bb_clearbit<1>(); // (CC1IE)

      ll_flight_capture_values[current_flight] = tof_timer::get()->ccr1;
   }

  // tof_timer::get()->sr = 0U;
}

namespace {
   // number of pulse sent this burst
   int pulsecount = 0;
}

void setup_ultrasonic_pulse_stream()
{
   setup_pulse_timer();
   current_flight = flight_north_south;
   set_transmit_address();
   
   pulsecount = 0;
   quan::stm32::set<led>();
   // starting the tof_timer also starts the pulse timer
   quan::stm32::enable<tof_timer>();
}

/*
pulse timer compare irq is used to count the number of pulses to send.
*/
extern "C" __attribute__ ((interrupt ("IRQ") ))
void TIM1_CC_IRQHandler()
{
   pulse_timer::get()->sr = 0U;
   ++pulsecount;
   if ( pulsecount == num_ultrasound_pulses){
       quan::stm32::disable<pulse_timer>();
       pulsecount = 0;
       set_receive_address();
   }
}

/*
get here from TIM1 Update interrupt
not enabled atm
*/
extern "C" __attribute__ ((interrupt ("IRQ") ))
void TIM1_UP_TIM16_IRQHandler()
{
   pulse_timer::get()->sr = 0;
}
