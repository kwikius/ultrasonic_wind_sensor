#ifndef WINDSENSOR_WIND_SENSOR_PACKET_HPP_INCLUDED
#define WINDSENSOR_WIND_SENSOR_PACKET_HPP_INCLUDED

#include <zapp4_crc.h>
#include <zapp4_do_crc.h>
#include <static_packet_parser.h>

#include <quan/angle.hpp>
#include <quan/velocity.hpp>

 enum class PacketID : uint16_t {
   WindSpeedAndDirection = 1U,
   Undef = 2U
 };
 
union velocity_and_direction_packet;

namespace zapp4{
    // required for backward compatibility  before static values wer allowed in unions

     // seed must be less than            4294967295
    template <> 
    struct get_crc_seed<velocity_and_direction_packet>
    {
       static constexpr uint32_t value = 1141577654U;
    };

    template <>
    struct get_PacketID<velocity_and_direction_packet>{
       static constexpr PacketID value = PacketID::WindSpeedAndDirection;
    };
 }

 union velocity_and_direction_packet{
      // seed must be less than            4294967295                                 
      //static constexpr uint32_t crc_seed = 1141577654U;
     // static constexpr PacketID packet_id = PacketID::WindSpeedAndDirection;

      velocity_and_direction_packet(
         quan::velocity_<float>::m_per_s const & speed,
         quan::angle_<float>::deg const & direction
      )
      : values{speed,direction}{ zapp4::do_crc(*this);}
      struct packet{
          packet(
            quan::velocity_<float>::m_per_s const &speed_in, 
            quan::angle_<float>::deg const & direction_in
          ):id{zapp4::get_PacketID<velocity_and_direction_packet>::value},
            reserved{0U},
            speed{speed_in},
            direction{direction_in}
          {}

          PacketID id;
          uint16_t reserved;
          quan::velocity_<float>::m_per_s speed;
          quan::angle_<float>::deg direction;
          uint32_t crc;
       } values;
       uint8_t ar[sizeof(values)];

      template <uint16_t MaxEncodedPacketLength>
      velocity_and_direction_packet(cobs::static_packet_parser<MaxEncodedPacketLength> const & parser)
      {
         static_assert(MaxEncodedPacketLength > sizeof(ar),"packet_parser buffer too short");
         memcpy(ar,parser.get_decoded_packet(),sizeof(ar));
      }
 };

static_assert ( sizeof (velocity_and_direction_packet) == 16,"unexpected union size");
static_assert ( (sizeof (velocity_and_direction_packet) % 4) == 0,"unexpected union size");

#endif // WINDSENSOR_WIND_SENSOR_PACKET_HPP_INCLUDED
