
#include <string.h>
#include "usart.h"
#include "adc.h"
#include <quan/conversion/float_to_ascii.hpp>
#include <quan/fixed_quantity/literal.hpp>
#include <quan/time.hpp>
#include <quan/velocity.hpp>
#include <quan/length.hpp>
#include <quan/area.hpp>
#include <quan/frequency.hpp>
#include <quan/acceleration.hpp>
#include <quan/angle.hpp>
#include <quan/fixed_quantity/operations/atan2.hpp>
#include <quan/two_d/vect.hpp>
#include "resource.hpp"
#include "windsensor_packet.hpp"

namespace {

   QUAN_QUANTITY_LITERAL(time,us)
   QUAN_QUANTITY_LITERAL(length,mm)
   QUAN_QUANTITY_LITERAL(area,mm2)
   QUAN_QUANTITY_LITERAL(velocity,m_per_s)
   QUAN_QUANTITY_LITERAL(acceleration,m_per_s2)
   QUAN_QUANTITY_LITERAL(frequency,Hz)
   QUAN_QUANTITY_LITERAL(angle,deg)
}

void setup();

void setup_ultrasonic_pulse_stream();

bool get_envelope_capture(quan::time::us (& result)[4]);

namespace
{
   // TODO: add a temperature sensor and use that to adjust sound velocity
   quan::velocity::m_per_s sound_velocity = 344.7_m_per_s;
   // measurement uncertainty of flight distance measurements
   // statistical variance or standard_deviation ^ 2

   // from a random sample of measurements
 //  quan::length::mm flight_distance_measurement_standard_deviation = 0.2_mm;
  // quan::time::us tof_measurement_std_dev = 0.8_us;

   // a random variable representing how much the wind varies
   quan::acceleration::m_per_s2 wind_accel_std_dev = 1_m_per_s2;

   // horz sensor to center distance
   quan::length::mm radius = 75_mm;

   quan::length::mm height = 128_mm;

   quan::angle::rad const flight_angle  = quan::atan2(height, radius);
   auto flight_distance = 2 * quan::sqrt(quan::pow<2>(radius) + quan::pow<2>(height));

   // mean error in random sample when measuring nominal windless condition
   // use the cal function to find this out
   // TODO auto cal?
   quan::time::us north_south_bias = 1.31_us;
   quan::length::mm north_south_distance = flight_distance;

   quan::time::us east_west_bias = 0.92_us;
   quan::length::mm east_west_distance = flight_distance;

   quan::frequency::Hz tof_update_frequency = 50_Hz/4.0;

   // Time taken in filter from actual start of pulse to comparator falling edge
   quan::time::us rise_time = 98_us;
   // std dev of risetime is probably base on  1/ ultrasound frequency
   //quan::time::us rise_time_std_dev = 98_us;

   quan::velocity::m_per_s solve_wind_velocity(  quan::time::us const & tF, quan::time::us const & tR)
   {
      if ( (tF > 1_us) && ( tR > 1_us) ){
          return flight_distance / (2 * cos(flight_angle)) * ( 1 / tF - 1 / tR);
      }else{
         return 0_m_per_s;
      }
   }

   quan::velocity::m_per_s solve_wind_velocity_north_south( quan::time::us const * results)
   {
      auto const tF = results[0] - (rise_time + north_south_bias/2.f);
      auto const tR = results[1] - (rise_time - north_south_bias/2.f);

      return solve_wind_velocity(tF, tR);
   }

   quan::velocity::m_per_s solve_wind_velocity_east_west( quan::time::us const * results)
   {
      auto const tF = results[0] - (rise_time + east_west_bias/2);
      auto const tR = results[1] - (rise_time - east_west_bias/2);

      return solve_wind_velocity(tF, tR);
   }

   // timing with no correction
   // for calibration in still conditions
   void raw_timing_output()
   {
      while (1) {
         quan::time::us results[4];
         while ( !get_envelope_capture(results)) { asm volatile ("nop":::);}

         auto output = [] (const char * str, quan::time::us const & t ){
            serial::port::write(str);
            char buf[50] = "";
            quan::float_to_ascii<3>(t.numeric_value(),buf);
            serial::port::write(buf);
            serial::port::write(" us\n");
            serial::port::flush_tx();
         };

         output("NS = ",results[0]);
         output("SN = ",results[1]);

         output("EW = ",results[2]);
         output("WE = ",results[3]);

         serial::port::write("----------\n");
      }  
   }


   void filtered_raw_timing_output(float filter_gain)
   {
      
      quan::time::us filtered_results [4] = {0_us};

      // initial estimate is initial measurement
      for ( uint32_t i = 0U; i < 2U; ++i){
         // ignore first result set
         while ( !get_envelope_capture(filtered_results)) { asm volatile ("nop":::);}
      }

      while (1) {
         quan::time::us results[4];
         while ( !get_envelope_capture(results)) { asm volatile ("nop":::);}

         for ( uint32_t i = 0; i < 4; ++i){
            filtered_results[i] += filter_gain * ( results[i] - filtered_results[i]);
         }

         auto output = [] (const char * str, quan::time::us const & t ){
            serial::port::write(str);
            char buf[50] = "";
            quan::float_to_ascii<3>(t.numeric_value(),buf);
            serial::port::write(buf);
            serial::port::write(" us\n");
            serial::port::flush_tx();
         };

         output("NS = ",filtered_results[0]);
         output("SN = ",filtered_results[1]);
         output("NE diff = ",filtered_results[0]- filtered_results[1]);
         output("EW = ",filtered_results[2]);
         output("WE = ",filtered_results[3]);
         output("EW diff = ",filtered_results[2]- filtered_results[3]);
         serial::port::write("----------\n");
      }  
   }

   // x is EastToWest and y is NorthToSouth
   // so [0 m/s,1 m/s] means from due north
   // [0 m/s,-1 m/s] from due south
   // [ 1 m/s, 0 m/s] from due east
   // [0 m/s , -1 m/s] fromdue west
   // result 0 deg means blowing from due north t
   // 90 deg means blowing from due east
   //etc
   quan::angle::deg get_wind_direction(quan::two_d::vect<quan::velocity::m_per_s> const & v)
   {
      quan::angle::deg wind_direction  = quan::atan2(v.x,v.y);

      if ( wind_direction < 0_deg){
         wind_direction += 360_deg;
      }
      return wind_direction;
   }

   quan::velocity::m_per_s get_wind_speed(quan::two_d::vect<quan::velocity::m_per_s> const & v)
   {
      return quan::two_d::magnitude(v);
   }

   struct wind_sensor{

      using wind_speed_vect_t = quan::two_d::vect<quan::velocity::m_per_s>;

      // blocks
      // call once at startup
      static void init()
      {
         quan::time::us results [4] = {0_us};
         //ignore first
         while ( !get_envelope_capture(results)) { asm volatile ("nop":::);}

         while ( !get_envelope_capture(results)) { asm volatile ("nop":::);}

         // initial estimate is same as measurement
         
         estimate = { 
            solve_wind_velocity_east_west(results + 2U) ,
            solve_wind_velocity_north_south(results)
         };
         // TODO: initial estimate_uncertainty = measurement_uncertainty
      }

      // blocks waiting for update : TODO RTOS
      static void update()
      {
         wind_speed_vect_t const prediction = estimate;

         quan::time::us results [4] = {0_us};
         while ( !get_envelope_capture(results)) { asm volatile ("nop":::);}

            wind_speed_vect_t measurement = { 
               solve_wind_velocity_east_west(results + 2U) ,
               solve_wind_velocity_north_south(results)
            };

         float const kalman_gain = 0.1; //estimate_uncertainty / (estimate_uncertainty + measurement_uncertainty);

         estimate = prediction + kalman_gain * ( measurement - prediction);

      }

      static quan::velocity::m_per_s get_wind_speed()
      {
         return ::get_wind_speed(estimate);
      }

      static quan::angle::deg get_wind_direction()
      {
         return ::get_wind_direction(estimate);
      }

    private:
         static wind_speed_vect_t estimate;         
   };

   // call init?
   wind_sensor::wind_speed_vect_t wind_sensor::estimate;

   template <typename Packet>
   void send_packet( Packet const & packet)
   {
      uint8_t encoded[quan::uav::cobs::get_encoded_length<sizeof(packet)>()];
      quan::uav::cobs::encode (packet.ar,sizeof(packet),encoded);

      serial::port::put('\0'); // frame
      serial::port::write(reinterpret_cast<char*>(encoded),sizeof(encoded));
      serial::port::put('\0'); // frame
   }

   template <typename Packet, typename... ValueTypes>
   void send_packet( ValueTypes const &... values )
   {
      Packet packet{values...};
      send_packet(packet);
   }

   void wind_velocity_output()
   {
      wind_sensor::init();
      uint32_t iter = 0U;
      for(;;){

         wind_sensor::update();

         ++iter;
         // show we are running
         if ((iter % 10 ) == 0){
             quan::stm32::complement<led>();
         }

         auto const speed = wind_sensor::get_wind_speed();
         auto const direction = wind_sensor::get_wind_direction();

         send_packet<velocity_and_direction_packet>(speed,direction);
      }
   }


}// namespace

int main(void)
{
  setup();

  serial::port::write("stm32L4 ultrasonic detector test\n");
  
  setup_ultrasonic_pulse_stream();

  wind_velocity_output();
  //filtered_raw_timing_output(0.1);
}


