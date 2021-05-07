#ifndef ARDUINO_ZAPP4_DECODE_PACKET_HPP_INCLUDED
#define ARDUINO_ZAPP4_DECODE_PACKET_HPP_INCLUDED

#include <crc_is_good.h>

namespace zapp4{

   template <typename Packet,typename Parser >
   inline bool decode_packet( uint16_t const packet_length, Parser const & parser,void (*pfn)( Packet const & packet) )
   {
      if ( packet_length == sizeof(Packet)){
         Packet packet {parser};
         if(crc_is_good(packet)){
            pfn(packet);
            return true;
         }
      }
      return false;
   }

} // app4

#endif // ARDUINO_ZAPP4_DECODE_PACKET_HPP_INCLUDED
