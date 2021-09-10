#include <cmath>
#include <quan/fixed_quantity/operations/fmod.hpp>
#include <quan/out/frequency.hpp>
#include <quan/out/voltage.hpp>
#include <quan/out/time.hpp>
#include <quan/constants/constant.hpp>
#include <quan/angle.hpp>
#include <quan/meta/integer_max.hpp>
#include <random>
#include <iostream>

/**  
 *@file sine wave modulated pulse waveform sim, for testing ultrasonic transducer detect algorithm
 **/

namespace {

   QUAN_QUANTITY_LITERAL(frequency, kHz);
   QUAN_QUANTITY_LITERAL(frequency, MHz);
   QUAN_QUANTITY_LITERAL(time,us);
   QUAN_QUANTITY_LITERAL(voltage,mV);

   auto ultrasound_frequency = 200.0_kHz;
   auto sampling_frequency = 1.0_MHz;

/*
  damped pulse envelope.
   The rise curve is modelled as an exponential curve
   Vtr(t) = Vmax * ( 1 - std::exp(-t/k1))

   The fall curve is modelled as an exponential curve
   starting at the end of the inputDuration
   Vt = Vtr(inputDuration) * std::exp(-(t - inputDuration) / K2);
*/
  
   struct dampedPulseEnvelope{
      quan::voltage::mV Vmax;// Max amplitude of envelope
      quan::time::us riseDuration; // time to rise to 0.67 of Vmax at start of pulse
      quan::time::us inputDuration; // Duration of the assumed excitation pulse stream. Must be longer than rise duration
      quan::time::us fallDuration;  // Duration for the pulse to fall to 0.33 of Vmax at end of inputDuration

       quan::voltage::V getAmplitude(quan::time::us const & t)const
       {
         if ( t <= inputDuration){
            return Vmax * (1 - std::exp(-t/riseDuration));
         }else{
            return getAmplitude(inputDuration) * std::exp(- (t- inputDuration) / fallDuration);
         }
      }
   };

   /**
      damped sine pulse with envelope
   **/
   struct damped_sine_pulse{
      quan::frequency::kHz const modulation_frequency;  // The frequency at which to modulate the pulse
      dampedPulseEnvelope const envelope;                // pulse envelope as above
      quan::time::us const start_time;                  // pulse start time
      quan::voltage::V getAmplitude(quan::time::us const & t)const
      {
         if ( t > start_time){
            quan::time::us const period = 1.0/modulation_frequency;
            auto const phase_pos = fmod(t,period);
            quan::angle::rad const omega = 2.0 * quan::constant::pi * phase_pos / period;
            return sin(omega) * envelope.getAmplitude(t-start_time);
         }else {
            return 0_mV;
         }
      }
   };

   struct noise_generator{
     quan::voltage::mV max_amplitude;
     quan::voltage::mV min_amplitude;
     quan::voltage::mV dc_offset;
     std::mt19937 gen32;

      quan::voltage::V getNoise()
      {
           auto const random = static_cast<int32_t>(gen32());
           auto const centre = (max_amplitude + min_amplitude)/2;
           return ((static_cast<double>(random) / quan::meta::integer_max<int32_t>::value) * (max_amplitude - min_amplitude)/2) - centre + dc_offset;
      }
   };
}

int main()
{
  dampedPulseEnvelope const envelope = { 1000_mV, 3_us,30_us,20_us};

  damped_sine_pulse const waveform = {ultrasound_frequency,envelope,16.7_us};

  quan::time::us const sample_period = 1.0 / sampling_frequency;

  noise_generator noise = { 50_mV,-50_mV, 0_mV};

  std::cout << "time, voltage\n";
  for (quan::time::us t = 0_us; t < 100_us;t += sample_period){
      auto amplitude = waveform.getAmplitude(t) + noise.getNoise();
      std::cout << t.numeric_value() << ", " << amplitude.numeric_value() << '\n';
  }
}