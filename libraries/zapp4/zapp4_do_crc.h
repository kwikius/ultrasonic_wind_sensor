#ifndef ARDUINO_ZAPP4_DO_CRC_HPP_INCLUDED
#define ARDUINO_ZAPP4_DO_CRC_HPP_INCLUDED

#include "zapp4_crc.h"

namespace zapp4{

 template <typename Packet>
 struct get_crc_seed;

 template <typename Packet>
 struct get_PacketID;

   template <typename Packet>
   inline void do_crc(Packet & packet)
   {
      packet.values.crc = get_crc_seed<Packet>::value;
      uint32_t constexpr n_iters = sizeof(Packet)/4U;
      uint32_t * arr = (uint32_t*) &packet.values.id;
      for ( uint32_t i = 0; i < n_iters; ++i){
         packet.values.crc = zapp4::crc32(packet.values.crc,arr[i]);
      }
   }

} 

#endif // ARDUINO_ZAPP4_DO_CRC_HPP_INCLUDED
