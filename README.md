

# Ultrasonic Wind Speed and Direction Sensor from a car reversing kit and an Arduino.


Ultrasonic wind sensors are known for their accuracy and reliability. However they also tend to be quite expensive.

The Ultrasonic wind sensor described here can be built for around £20($20) and has excellent performance.

The main components of the project are an Ebay car reversing kit, available for around £9(10$US) and an arduino Pro Mini or clone for around 5$US.

Click the below image to watch a video of the project on YouTube:

[![Youtube - Ultrasonic Wind Speed and Direction Sensor from a car reversing kit](http://img.youtube.com/vi/_kloM0Tk8lo/2.jpg)](https://www.youtube.com/watch?v=_kloM0Tk8lo&list=PL-N-nQ0dVeh7bO8eM10oHQ2tNhluuLc5f 
"Youtube - Ultrasonic Wind Speed and Direction Sensor from a car reversing kit")

Full details about building the electronics and mechanical hardware can be found at Hackaday.io

https://hackaday.io/project/181677-wind-sensor-using-car-reversing-kit

The project is designed to be managed in the Arduino IDE.

### Installing and building the project

Firstly clone the Github Project

```>$ git clone https://github.com/kwikius/ultrasonic_wind_sensor.git ```

Next install the quan-trunk submodule using the following command

```>$ git submodule update --init --recursive```

-------------------------

### Building and uploading the Atmega326 part of the Project

Start the Arduino IDE. Click on 'File > Preferences' and modify the sketchbook location so that it leads to the ultrasonic_wind_sensor directory.
Close Arduino and reopen.

When Arduino reopens , click on 'File > Open' , navigate to the ultrasonic_wind_sensor/ArduUltrasonicWindSensor subdirectory and select "ArduUltrasonicWindSensor.ino".

Next click on 'Tools > Board' and select   (AVR boards) > "Arduino Pro or Pro Mini".

Next click on 'Tools > Processor' and select "ATmega328P(5v, 16 MHz)"

Now select 'Sketch > Verify/Compile' to compile. 

The code can be uploaded by attaching a serial cable to the Arduino Pro Mini and clicking Sketch > Upload.

By default the sketch provides text output via the serial port.

For [web server](https://github.com/kwikius/ultrasonic_wind_sensor/blob/master/web_server/web_server.ino) ,
uncomment //#define WIND_SENSOR_WEB_SERVER_OUTPUT in ArduUltrasonicWindSensor.ino

------------------------------

TODO Proper installation instructions for [web server](https://github.com/kwikius/ultrasonic_wind_sensor/blob/master/web_server/web_server.ino)

### Building and uploading the web server.

Assumes you are using the ESP-01. You will need a suitable programming fixture, 
[such as described here](https://www.allaboutcircuits.com/projects/flashing-the-ESP-01-firmware-to-SDK-v2.0.0-is-easier-now/)

You also need to install the ESP8266 plugin for Arduino if not already installed, 
[as described here](https://create.arduino.cc/projecthub/electropeak/getting-started-w-nodemcu-esp8266-on-arduino-ide-28184f)

You need the following libraries in libraries subdirectory 
* https://github.com/me-no-dev/ESPAsyncTCP
* https://github.com/me-no-dev/ESPAsyncWebServer
* https://github.com/Links2004/arduinoWebSockets

Start the Arduino IDE. Click on 'File > Preferences' and modify the sketchbook location so that it leads to the ultrasonic_wind_sensor directory.
Close Arduino and reopen.

When Arduino reopens , click on 'File > Open' , navigate to the ultrasonic_wind_sensor/web_server subdirectory 
and select "web_server.ino".

Next click on 'Tools > Processor' and select "Generic ESP8266 Module"

Now select 'Sketch > Verify/Compile' to compile. 

To Upload the code, connect the programming fixture via 3.3V Serial cable to your PC.
Insert the ESP-01 in the fixture.
Hold down both the Flash and Reset buttons on the programming fixture with the ESP-01 inserted.
Release the Reset button while keeping Flash button pressed. 

While keeping Flash button pressed. Select 'Sketch > Upload'.

Watch the output from the Arduino IDE. Once it is apparent that the code is uploading, you can release the Flash button, 
as it is only required to be pressed when the ESP-01 boots.

For initial programming of the webserver you will need to use the programming fixture to set the Network parameters.

The WebServer requires the nextwork ID and password to connect to. If there are no saved parameters the web server will ask for those.
N.B If using the Arduino Serial monitor for Serial IO, then set Carriage return as the end of line type and set baud rate at 115,200 baud.
You have the choice to save the network parameters in EEPROM or not. If you choose not to save the parameters, they will be available 
until the web server is reset. If you choose to save the parameters they will be saved in EEPROM and will be used by default at reset.
You can change the network you connect to by pressing Return ('\r') three times in 1st 5 seconds after reset. 
Again if you wish to save the new settings the old network settings will be overwritten, otherwise the settings you entered will be used 
unitil next reset.

Once the network parameters have been saved in EEprom, you can put the ESP-01 back in the wind sensor board.

(Note that you must be running the modified ArduUltrasonicWindSensor.ino for Atmega328 to talk to the webserver as described above)

With the wind sensor powered up, you should see the ESP-01 LED blinking. Once the LED goes out, it indicates the web server is connected to the network

The webserver is designed to support MDNS ( NOTE: Unfortunately this doesnt work in Android)

Open a web browser and enter "windsensor.local" as the URL. 
All being well, you should see the windsensor web page with real time data streaming from the wind sensor.

For Android, you will need to download an App such as [Fing](https://www.fing.com/products/fing-app)













