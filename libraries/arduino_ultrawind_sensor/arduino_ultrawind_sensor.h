#ifndef ARDUINO_ULTRAWIND_SENSOR_H_INCLUDED
#define ARDUINO_ULTRAWIND_SENSOR_H_INCLUDED

#include <quan.h>
#include <quan/time.hpp>

// Read the latest capture set.
bool get_ultrasound_capture(quan::time::us (& result)[4]);
void txPulseInitialSetup();

#endif // ARDUINO_ULTRAWIND_SENSOR_H_INCLUDED
