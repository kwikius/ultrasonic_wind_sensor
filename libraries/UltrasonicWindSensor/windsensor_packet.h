#ifndef WINDSENSOR_WIND_SENSOR_PACKET_HPP_INCLUDED
#define WINDSENSOR_WIND_SENSOR_PACKET_HPP_INCLUDED

#include <quan/tracker/zapp4/crc.hpp>
#include <quan/tracker/zapp4/do_crc.hpp>
#include <quan/tracker/zapp4/decode_packet.hpp>
#include <quan/uav/cobs/static_packet_parser.hpp>
#include <quan/velocity.hpp>
#include <quan/angle.hpp>
#include <polled_serial.h>

 enum class PacketID : uint16_t {
   WindSpeedAndDirection = 1U,
   Undef = 2U
 };

 union velocity_and_direction_packet{
      // seed must be less than            4294967295                                 
      static constexpr uint32_t crc_seed = 1141577654U;
      static constexpr PacketID packet_id = PacketID::WindSpeedAndDirection;

      velocity_and_direction_packet(quan::velocity_<float>::m_per_s const &speed, quan::angle_<float>::deg const & direction)
      : values{speed,direction}{ quan::tracker::zapp4::do_crc(*this);}
      struct packet{
          packet(quan::velocity_<float>::m_per_s const &speed_in, quan::angle_<float>::deg const & direction_in)
          :id{packet_id},
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
      velocity_and_direction_packet(quan::uav::cobs::static_packet_parser<MaxEncodedPacketLength> const & parser)
      {
         static_assert(MaxEncodedPacketLength > sizeof(ar),"packet_parser buffer too short");
         memcpy(ar,parser.get_decoded_packet(),sizeof(ar));
      }
 };

static_assert ( sizeof (velocity_and_direction_packet) == 16,"unexpected union size");
static_assert ( (sizeof (velocity_and_direction_packet) % 4) == 0,"unexpected union size");

// send packet using zapp4 protocol
   template <typename Packet>
   inline void send_packet( Packet const & packet)
   {
      uint8_t encoded[quan::uav::cobs::get_encoded_length<sizeof(packet)>()];
      quan::uav::cobs::encode (packet.ar,sizeof(packet),encoded);

      polledSerial.write('\0'); // frame
      polledSerial.write(reinterpret_cast<char*>(encoded),sizeof(encoded));
      polledSerial.write('\0'); // frame
   }

   template <typename Packet, typename... ValueTypes>
   inline void send_packet( ValueTypes const &... values )
   {
      Packet packet{values...};
      send_packet(packet);
   }

#endif // WINDSENSOR_WIND_SENSOR_PACKET_HPP_INCLUDED
