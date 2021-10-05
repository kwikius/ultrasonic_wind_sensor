
#include "wind_sensor.h"
#include <quan/atan2.hpp>
#include <arduino_ultrawind_sensor.h>

namespace {

   QUAN_QUANTITY_LITERAL(time,us)
   QUAN_QUANTITY_LITERAL(length,mm)
   QUAN_QUANTITY_LITERAL(velocity,m_per_s)
   QUAN_QUANTITY_LITERAL(angle,deg)
}

wind_sensor_t::wind_sensor_t(quan::length::mm const & transducer_radius_in,quan::length::mm sensor_height_in)
: m_transducer_radius{transducer_radius_in},
  m_sensor_height{sensor_height_in},
  m_north_south_bias{0_us},
  m_east_west_bias{0_us},
  m_rise_time{100_us}
{}

void wind_sensor_t::init()
{
   txPulseInitialSetup();

   // TODO set in eeprom
   this->setNorthSouthBias(0_us);
   this->setEastWestBias(0_us);

   quan::time::us results [4] = {0_us};
   //ignore first
   while ( !get_ultrasound_capture(results)) { asm volatile ("nop":::);}

   while ( !get_ultrasound_capture(results)) { asm volatile ("nop":::);}

   // initial estimate is same as measurement
   
   m_estimate = { 
      solve_wind_velocity_east_west(results + 2U) ,
      solve_wind_velocity_north_south(results)
   };
   // TODO: initial estimate_uncertainty = measurement_uncertainty
}

void wind_sensor_t::update()
{
   wind_speed_vect_t const prediction = m_estimate;

   quan::time::us results [4] = {0_us};
   while ( !get_ultrasound_capture(results)) { asm volatile ("nop":::);}

      wind_speed_vect_t measurement = { 
         solve_wind_velocity_east_west(results + 2U) ,
         solve_wind_velocity_north_south(results)
      };

   float const kalman_gain = 0.1; //estimate_uncertainty / (estimate_uncertainty + measurement_uncertainty);

   m_estimate = prediction + kalman_gain * ( measurement - prediction);
}

quan::velocity::m_per_s 
wind_sensor_t::getWindSpeed()const
{
   return ::get_wind_speed(m_estimate);
}

quan::angle::deg 
wind_sensor_t::getWindDirection() const
{
   return ::get_wind_direction(m_estimate);
}

quan::velocity::m_per_s 
wind_sensor_t::solve_wind_velocity(  quan::time::us const & tF, quan::time::us const & tR)const
{
   if ( (tF > 1_us) && ( tR > 1_us) ){
      return m_transducer_radius * ( 1 / tF - 1 / tR);
   }else{
      return 0_m_per_s;
   }
}

quan::velocity::m_per_s 
wind_sensor_t::solve_wind_velocity_north_south(quan::time::us const * results) const
{
   auto const tF = results[0] - (m_rise_time + m_north_south_bias/2);
   auto const tR = results[1] - (m_rise_time - m_north_south_bias/2);

   return solve_wind_velocity(tF, tR);
}

quan::velocity::m_per_s 
wind_sensor_t::solve_wind_velocity_east_west( quan::time::us const * results ) const
{
   auto const tF = results[0] - (m_rise_time + m_east_west_bias/2);
   auto const tR = results[1] - (m_rise_time - m_east_west_bias/2);

   return solve_wind_velocity(tF, tR);
}

quan::velocity::m_per_s 
wind_sensor_t::get_sound_velocity()
{
   return 344.7_m_per_s;
}

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

