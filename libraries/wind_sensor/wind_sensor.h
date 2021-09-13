#ifndef ULTRASONIC_WIND_SENSOR_H_INCLUDED
#define ULTRASONIC_WIND_SENSOR_H_INCLUDED

#include <quan.h>
#include <quan/length.hpp>
#include <quan/time.hpp>
#include <quan/velocity.hpp>
#include <quan/angle.hpp>
#include <quan/two_d/vect.hpp>

quan::velocity::m_per_s get_wind_speed(quan::two_d::vect<quan::velocity::m_per_s> const & v);
quan::angle::deg get_wind_direction(quan::two_d::vect<quan::velocity::m_per_s> const & v);

struct wind_sensor_t{

   wind_sensor_t(quan::length::mm const & transducer_radius_in,quan::length::mm sensor_height_in);

   void init();

   void update();

   quan::velocity::m_per_s getWindSpeed()const;
   quan::angle::deg getWindDirection() const;

   using wind_speed_vect_t = quan::two_d::vect<quan::velocity::m_per_s>;

   /**
    * @brief get velocity of sound at the sensor
    *    Here just returning a constant
    * could be free
   **/
   static quan::velocity::m_per_s get_sound_velocity();

   quan::time::us getNorthSouthBias()const {return m_north_south_bias;}

   quan::time::us getEastWestBias()const {return m_east_west_bias; }

   void setNorthSouthBias(quan::time::us const & t) { m_north_south_bias = t; }

   void setEastWestBias(quan::time::us const & t) { m_east_west_bias = t; }
   
   /**
    * @brief distance from transducer to central axis on horizontal part
    **/ 
   quan::length::mm const m_transducer_radius;
   /**   
    * @brief height from transducer to sound reflecting pad
    **/
   quan::length::mm const m_sensor_height;
   quan::angle::rad const m_flight_angle;
   QUAN_FLOAT_TYPE const m_cos_flight_angle;
   quan::length::mm const m_flight_distance;
private:

   quan::velocity::m_per_s solve_wind_velocity(quan::time::us const & tF, quan::time::us const & tR)const;
   quan::velocity::m_per_s solve_wind_velocity_north_south(quan::time::us const *results) const;
   quan::velocity::m_per_s solve_wind_velocity_east_west( quan::time::us const *results) const;

   quan::time::us m_north_south_bias;
   quan::time::us m_east_west_bias;
   quan::time::us m_rise_time;

   wind_speed_vect_t m_estimate;
};

#endif // ULTRASONIC_WIND_SENSOR_H_INCLUDED
