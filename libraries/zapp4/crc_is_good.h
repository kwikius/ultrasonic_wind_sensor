#ifndef ARDUINO_ZAPP4_CRC_IS_GOOD_HPP_INCLUDED
#define ARDUINO_ZAPP4_CRC_IS_GOOD_HPP_INCLUDED

#include <zapp4_do_crc.h>

namespace zapp4{

   template <typename Packet>
   inline
   bool crc_is_good(Packet & packet)
   {
      uint32_t const crc_read = packet.values.crc;
      do_crc(packet);
      return packet.values.crc == crc_read;
   }

} // quan::tracker::zapp4

#endif // ARDUINO_ZAPP4_CRC_IS_GOOD_HPP_INCLUDED
