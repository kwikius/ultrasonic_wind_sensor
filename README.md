

# Ultrasonic Wind Speed and Direction Sensor from a car reversing kit and an Arduino.


Ultrasonic wind sensors are known for their accuracy and reliability. However they also tend to be quite expensive.

The Ultrasonic wind sensor described here can be built for around £20($20) and has excellent performance.

The main components of the project are an Ebay car reversing kit, available for around £9(10$US) and an arduino Pro Mini or clone for around 5$US.

More details about building the electronics and mechanical hardware can be found at Hackaday.io

https://hackaday.io/project/181677-wind-sensor-using-car-reversing-kit

Click the below image to watch a video of the project on YouTube:

[![Youtube - Ultrasonic Wind Speed and Direction Sensor from a car reversing kit](http://img.youtube.com/vi/_kloM0Tk8lo/2.jpg)](https://www.youtube.com/watch?v=_kloM0Tk8lo 
"Youtube - Ultrasonic Wind Speed and Direction Sensor from a car reversing kit")

The project is designed to run in the Arduino IDE.

### Installing and building the project

Firstly clone the Github Project

```>$ git clone https://github.com/kwikius/ultrasonic_wind_sensor.git ```

Next install the quan-trunk submodule using the following command

```>$ git submodule update --init --recursive```

Start the Arduino IDE. Click on 'File > Preferences' and modify the sketchbook location so that it leads to the ultrasonic_wind_sensor directory.
Close Arduino and reopen.

When Arduino reopens , click on 'File > Open' , navigate to the ultrasonic_wind_sensor/ArduUltrasonicWindSensor subdirectory and select "ArduUltrasonicWindSensor.ino".

Next click on 'Tools > Board' and select   (AVR boards) > "Arduino Pro or Pro Mini".

Next click on 'Tools > Processor' and select "ATmega328P(5v, 16 MHz)"

Now select 'Sketch > Verify/Compile' to compile. 

The code can be uploaded by attaching a serial cable to the Arduino Pro Mini and clicking Sketch > Upload.





