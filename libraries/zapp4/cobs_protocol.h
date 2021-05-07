#ifndef ARDUINO_ZAPP4_COBS_PROTOCOL_HPP_INCLUDED
#define ARDUINO_ZAPP4_COBS_PROTOCOL_HPP_INCLUDED

/*
 Copyright (c) 2013 Andy Little 

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>
*/

/*
See stuartcheshire.org/papers/COBSforToN.pdf for more info about the COBS protocol
*/

#include <cstdint>

 namespace cobs{

   // N.B. The encoded length of the data not including the unencoded framing 0 byte
   template <uint8_t RawLength>
   constexpr inline uint8_t get_encoded_length()
   {
      static_assert(RawLength < 253, "data too long");
      return RawLength + 1;
   }
//!-!-!start-example
   // Length is length of the unencoded data, which must be less than 253 bytes
   // N.B. data does not include the unencoded framing 0 byte
   // length of the array src is assumed to be Length
   // length of the array dest is assumed to be get_encoded_length(length)
   // This function does not add the unencoded framing 0 byte

   inline bool encode(uint8_t const *src, uint8_t src_length, uint8_t *dest)
   {
      if ( src_length < 253){
         uint8_t *code_ptr = dest++;
         uint8_t code = 0x01;
         for(uint8_t const * const src_end = src + src_length;src < src_end;++src){

            if (*src ){
                *dest++ = *src;
                ++code;
            }else{
               *code_ptr = code;
               code_ptr = dest++;
               code = 0x01;
            }

//            if (*src == 0){
//               *code_ptr = code;
//               code_ptr = dest++;
//               code = 0x01;
//            }
//            else {
//               *dest++ = *src;
//                ++code;
//               if (++code == 0xFF) {
//                 *code_ptr = code;
//                  code_ptr = dest++;
//                  code = 0x01;
//               } 
//            }
         }
         *code_ptr = code;
         return true;
      }else{
         return false;
      }
   }
 //!-!-!end-example  
   // length is length of the encoded data, which must be less than 254 bytes
   // data does not include the unencoded framing 0 byte
   // length of the array src is assumed to be   length
   // length of the array dest is assumed to be length -1
   // returns true if the data was decoded or 
   // false if it is detected that the decoding process
   // would put data outside the dest buffer indicating corrupted data
   inline bool decode(uint8_t const *src, uint8_t const src_length, uint8_t *dest)
   {
      if ( src_length < 254){
         uint8_t const * const src_end = src + src_length -1;
         uint8_t const * const dest_end = dest + src_length -1;
         while (src < src_end){
            for (uint8_t i=1, code = *src++; i<code; ++i) {
             if ( dest < dest_end){
               *dest++ = *src++;
               }
               else {
                  return false;
               }
            }
            if ( dest < dest_end){
               *dest++ = 0;
            }
         }
         return true;
      }else{
         return false;
      }
   }

}// cobs

#endif //ARDUINO_ZAPP4_COBS_PROTOCOL_HPP_INCLUDED
