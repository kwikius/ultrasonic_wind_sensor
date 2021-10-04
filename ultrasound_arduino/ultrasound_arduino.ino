

#include <builtin_led.h>
#include <arduino_ultrawind_sensor.h>
#include <wind_sensor.h>
#include <windsensor_packet.h>

namespace {

  QUAN_QUANTITY_LITERAL( length,mm)
  QUAN_QUANTITY_LITERAL( time,us)

  wind_sensor_t windSensor{150_mm,250_mm};

}

void setup()
{
   Serial.begin(115200);
   builtin_led_setup();
   turn_off_builtin_led();

   txPulseInitialSetup();

   Serial.println("ultrasound wind sensor");

   // TODO set in eeprom
   windSensor.setNorthSouthBias(0_us);
   windSensor.setEastWestBias(0_us);

   windSensor.init();

   Serial.println("wind sensor initialised");

}

namespace {

   void readWindSensor()
   {
       quan::velocity::m_per_s const wind_speed = windSensor.getWindSpeed();
       quan::angle::deg wind_direction = windSensor.getWindDirection();

       Serial.print("wind speed = ");
       Serial.print(wind_speed.numeric_value());
       Serial.print(" m/s");

       Serial.print(", direction = ");
       Serial.print(wind_direction.numeric_value());
       Serial.println(" deg");
   }

// send packet using zapp4 protocol
   template <typename Packet>
   void send_packet( Packet const & packet)
   {
      uint8_t encoded[quan::uav::cobs::get_encoded_length<sizeof(packet)>()];
      quan::uav::cobs::encode (packet.ar,sizeof(packet),encoded);

      Serial.write('\0'); // frame
      Serial.write(reinterpret_cast<char*>(encoded),sizeof(encoded));
      Serial.write('\0'); // frame
   }

   template <typename Packet, typename... ValueTypes>
   void send_packet( ValueTypes const &... values )
   {
      Packet packet{values...};
      send_packet(packet);
   }

   void sendWindSensor() 
   {
      auto const speed = windSensor.getWindSpeed();
      auto const direction = windSensor.getWindDirection();

      send_packet<velocity_and_direction_packet>(speed,direction);
   }
}

void loop()
{
    windSensor.update();

    // uncomment for simple text output to serial
    readWindSensor();

    // framed packet protocol
    //sendWindSensor();
}

