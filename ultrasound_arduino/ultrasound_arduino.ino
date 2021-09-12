

#include <builtin_led.h>
#include <arduino_ultrawind_sensor.h>


namespace {

}

void setup()
{
   Serial.begin(115200);
   builtin_led_setup();
   turn_off_builtin_led();

   txPulseInitialSetup();

   Serial.println("ultrasound wind sensor starting");

   
   
}

namespace {
   void print_raw(quan::time::us(&durations)[4])
   {
      for ( uint8_t i = 0U; i < 4U;++i){
         Serial.print(durations[i].numeric_value());
         if ( i < 3){
            Serial.print( " us, ");
         }else{
            Serial.println(" us");
         }
      }
   }

   
}

void loop()
{
   quan::time::us durations[4];

   if( get_ultrasound_capture(durations)){
       print_raw(durations);
   }
}

