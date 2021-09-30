#ifndef ARDUINO_ULTRAWIND_SENSOR_H_INCLUDED
#define ARDUINO_ULTRAWIND_SENSOR_H_INCLUDED

#include <quan.h>
#include <quan/time.hpp>

void txPulseInitialSetup();

// Read the latest capture set.
// N.B. If return strue stateful
// and enables 
bool get_ultrasound_capture(quan::time::us (& result)[4]);

#endif // ARDUINO_ULTRAWIND_SENSOR_H_INCLUDED
