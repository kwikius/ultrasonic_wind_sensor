#ifndef ARDUINO_ZAPP4_COBS_STATIC_PACKET_PARSER_HPP_INCLUDED
#define ARDUINO_ZAPP4_COBS_STATIC_PACKET_PARSER_HPP_INCLUDED

#include <cstdint>
#include "cobs_protocol.h"

namespace cobs{

   template <uint16_t MaxEncodedPacketLength>
   struct static_packet_parser{
      static constexpr uint16_t max_packet_length = MaxEncodedPacketLength - 1U;
      enum class parser_state_t : uint8_t { not_synced, in_sync, in_packet};

      // allocate (2 * max_encoded_packet_length_in)- 1 on heap
      // for buffers
      static_packet_parser()
//      : m_encoded_packet_buffer{new uint8_t [max_encoded_packet_length_in]}
//      , m_decoded_packet_buffer{new uint8_t [max_encoded_packet_length_in-1]}
      : m_num_errors{0}
//      , m_max_encoded_packet_length{max_encoded_packet_length_in}
      , m_packet_idx{0}
      , m_current_packet_mode{parser_state_t::not_synced}
      
      { }

      ~static_packet_parser()
      {
         //delete [] m_encoded_packet_buffer;
      }

      // a pointer to the decoded packet
      uint8_t const * get_decoded_packet()const 
      {
         return m_decoded_packet_buffer;
      }

      uint32_t get_num_errors() const
      {
         return m_num_errors;
      }

      uint32_t clear_errors()
      {
         uint32_t result = m_num_errors;
         m_num_errors = 0;
         return result;
      }

      // parse the input
      // a zero return means still processing
      // a non-zero return means a packet is available
      // The return value is the length of the packet
      uint16_t parse(uint8_t ch)
      {
         switch (m_current_packet_mode){
            case parser_state_t::in_packet:{
               if ( ch == 0){ // new packet
                  if (cobs::decode(m_encoded_packet_buffer,m_packet_idx,m_decoded_packet_buffer)){
                     m_current_packet_mode = parser_state_t::in_sync;
                     return m_packet_idx -1;
                  }else{
                     ++ m_num_errors;
                     m_current_packet_mode = parser_state_t::not_synced;  
                     return 0;
                  }
               }else{ // more input to packet
                  if ( m_packet_idx < MaxEncodedPacketLength){
                     m_encoded_packet_buffer[m_packet_idx] = ch;
                     ++m_packet_idx;
                      return 0;  
                  }else{
                     ++ m_num_errors;
                     m_current_packet_mode = parser_state_t::not_synced;
                     return 0;  
                  }
               }
            }

            case parser_state_t::in_sync:{ // can have as many zeros as like between frames
               if ( ch != 0){
                  m_encoded_packet_buffer[0] = ch;
                  m_packet_idx = 1;
                  m_current_packet_mode = parser_state_t::in_packet;
               }
               return 0;
            }

            case parser_state_t::not_synced: {
               if ( ch == 0){
                  m_current_packet_mode = parser_state_t::in_sync;
               }
               return 0;
            }

            default:{ // shouldnt get here
               return 0;
            }
         }
      }
   private:

      uint32_t m_num_errors;
    //  uint16_t const m_max_encoded_packet_length;
      uint16_t m_packet_idx;
      parser_state_t  m_current_packet_mode; //  = not_synced;
      uint8_t m_encoded_packet_buffer[MaxEncodedPacketLength];
      uint8_t m_decoded_packet_buffer[MaxEncodedPacketLength-1];
      
   };

}//cobs


#endif // ARDUINO_ZAPP4_COBS_STATIC_PACKET_PARSER_HPP_INCLUDED
