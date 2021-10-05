

#include <builtin_led.h>
#include <wind_sensor.h>
#include <windsensor_packet.h>

namespace {

  /// Allows use of user defined literals defining lengths in mm
  QUAN_QUANTITY_LITERAL( length,mm)

  wind_sensor_t windSensor{150_mm,250_mm};

}

void setup()
{
   Serial.begin(115200);

   builtin_led_setup();

   Serial.println("ultrasonic wind sensor");

   windSensor.init();

   Serial.println("wind sensor initialised");

}

namespace {

   // send data as user readable
   void readWindSensor()
   {
       quan::velocity::m_per_s const wind_speed = windSensor.getWindSpeed();
       quan::angle::deg const wind_direction = windSensor.getWindDirection();

       Serial.print("wind speed = ");
       Serial.print(wind_speed.numeric_value());
       Serial.print(" m/s");

       Serial.print(", direction = ");
       Serial.print(wind_direction.numeric_value());
       Serial.println(" deg");
   }

   // send data as a binary packet
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
    //readWindSensor();

    // framed packet protocol
    sendWindSensor();
}

