#ifndef ARDUINO_ESP8266_LIBRARIES_USER_IN_H_INCLUDED
#define ARDUINO_ESP8266_LIBRARIES_USER_IN_H_INCLUDED

/*
   -1 error
   0 good
   above 0 other error
   
   int get_network_params( network_params & 
*/

const char* get_wifi_network_ssid();
const char* get_wifi_password();

bool get_network_params();

void flush_serial_input_buffer();

bool do_setup();
bool check_do_setup();

#endif
