

# Ultrasonic Wind Speed and Direction Sensor from a car reversing kit and an Arduino.


Ultrasonic wind sensors are known for their accuracy and reliability. However they also tend to be quite expensive.

The Ultrasonic wind sensor described here can be built for around £20($20) and has excellent performance.

The main components of the project are an Ebay car reversing kit, available for around £9(10$US) and an arduino Pro Mini or clone for around 5$US.

Click the below image to watch a video of the project on YouTube:

[![Youtube - Ultrasonic Wind Speed and Direction Sensor from a car reversing kit](http://img.youtube.com/vi/_kloM0Tk8lo/2.jpg)](https://www.youtube.com/watch?v=_kloM0Tk8lo&list=PL-N-nQ0dVeh7bO8eM10oHQ2tNhluuLc5f 
"Youtube - Ultrasonic Wind Speed and Direction Sensor from a car reversing kit")

Full details about building the electronics and mechanical hardware can be found at Hackaday.io

https://hackaday.io/project/181677-wind-sensor-using-car-reversing-kit

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

By default the sketch provides text output via the serial port.

------------------------------

TODO Proper installation instructions for [web server](https://github.com/kwikius/ultrasonic_wind_sensor/blob/master/web_server/web_server.ino)

Optional :
For [web server](https://github.com/kwikius/ultrasonic_wind_sensor/blob/master/web_server/web_server.ino) ,
uncomment //#define WIND_SENSOR_WEB_SERVER_OUTPUT in ArduUltrasonicWindSensor.ino

You need the following libraries in libraries subdirectory 
* https://github.com/me-no-dev/ESPAsyncTCP
* https://github.com/me-no-dev/ESPAsyncWebServer
* https://github.com/Links2004/arduinoWebSockets







