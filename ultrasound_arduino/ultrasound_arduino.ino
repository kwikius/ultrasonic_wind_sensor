

#include <builtin_led.h>
#include <arduino_ultrawind_sensor.h>
#include <wind_sensor.h>


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
}

void loop()
{
    windSensor.update();

    readWindSensor();
}

