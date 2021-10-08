
#include <builtin_led.h>
#include <wind_sensor.h>
#include <windsensor_packet.h>
#include <polled_serial.h>

namespace {

  /// Allows use of user defined literals defining lengths in mm
  QUAN_QUANTITY_LITERAL( length,mm)

  //instantiate the wind sensor class
  wind_sensor_t windSensor{150_mm,250_mm};
}

/** ###################################################################################
N.B To get accurate timing in interrupts, we dont use the standard Arduino.Serial, 
but rather  custom polledSerial object of PolledSerial class which doesn't use any interrupts.
Note that polledSerial will block while writing data, but this is not a problem ,
since the program is spending most time waiting for the wind sensor to take readings anyway.
A new reading is returned around every 1/50th sec.  

At 115200 baud we can write a maximum of
115200/(10*50) ~= 230 bytes of text or data per sensor reading

in binary mode assume we send 12 bytes including frames, checksum etc. then
the minimum baud rate = 12*10 * 50 ~= 6000 baud so say min of 9600 baud
#######################################################################################
**/
void setup()
{
   polledSerial.begin(115200);
 
   builtin_led_setup();

   polledSerial.println("ultrasonic wind sensor initialising...");

   windSensor.init();

   polledSerial.println("wind sensor initialised");

}

namespace {

   // send data as user readable over serial port
   void readWindSensor()
   {
       quan::velocity::m_per_s const wind_speed = windSensor.getWindSpeed();
       quan::angle::deg const wind_direction = windSensor.getWindDirection();

       polledSerial.print("wind speed = ");
       polledSerial.print(wind_speed.numeric_value());
       polledSerial.print(" m/s");

       polledSerial.print(", direction = ");
       polledSerial.print(wind_direction.numeric_value());
       polledSerial.println(" deg");
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
    // uncomment for simple text output to serial port
    //readWindSensor();
    // uncomment framed packet protocol used by web server
    sendWindSensor();
}

