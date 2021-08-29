#ifndef ARDUINO_ESP8266_LIBRARIES_USER_IN_H_INCLUDED
#define ARDUINO_ESP8266_LIBRARIES_USER_IN_H_INCLUDED

/**
* @brief Send message re user input on serial port.
*  Wait for 5 secs to see if user has pressed return X3
*  If so get network data from user
*  else get settings from eeprom if available
* @return 
**/
void setup_network_params();

/**
* @brief get the network SSID
**/
const char* get_wifi_network_ssid();

/**
* @brief get the network password
**/
const char* get_wifi_password();

#endif
