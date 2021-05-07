

#include <quan/serial_port.hpp>
#include <iostream>
#include </home/andy/cpp/projects/stm32l4_v1/cube_generated/Inc/windsensor_packet.hpp>
#include <quan/tracker/zapp4/decode_packet.hpp>
#include <quan/out/velocity.hpp>
#include <quan/out/angle.hpp>

   void output_decoded(velocity_and_direction_packet const & p)
   {
      std::cout << " v = " << p.values.speed << ", d = " << p.values.direction <<'\n';
   };

int main()
{

   quan::serial_port sp("/dev/ttyACM0");

   sp.init(B115200);

   if (sp.good()){
      std::cout << "serial port opened successfully\n";
   }else{
      std::cout << "serial port open failed\n";
      return 1;
   }

   quan::uav::cobs::static_packet_parser<
      quan::uav::cobs::get_encoded_length<sizeof(velocity_and_direction_packet)>()
   > parser;

   for(;;){
      if(sp.in_avail() ){ 
         uint8_t ch ;
         sp.read(&ch,1);
         uint16_t const packet_length = parser.parse(ch);
         if ( packet_length > 0U ){
            uint8_t const * decoded_packet_buffer = parser.get_decoded_packet();
            uint16_t const packet_id = static_cast<uint16_t>(decoded_packet_buffer[0]) 
            + (static_cast<uint16_t>(decoded_packet_buffer[1]) << 8U);
            if ( static_cast<PacketID>(packet_id) == PacketID::WindSpeedAndDirection){
               if ( !quan::tracker::zapp4::decode_packet<velocity_and_direction_packet>(
                     packet_length,parser,output_decoded
                  ) == true
               ) {
                  std::cout << "parse packet failed\n";
               }
            }
         }
      }
   }
}