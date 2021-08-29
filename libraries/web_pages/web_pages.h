#ifndef ARDUINO_ESP8266_WEB_PAGES_H_INCLUDED
#define ARDUINO_ESP8266_WEB_PAGES_H_INCLUDED

#include <Arduino.h>

#if defined ESP8266
   #include <ESP8266WiFi.h>
#else
   #if defined ESP32
      #include <WiFi.h>
   #endif
#endif

void serve_page(WiFiClient & client, const char* mime_type, const char* text);

const char* get_web_page_compass_js();

const char* get_web_page_main_html();

#endif //ARDUINO_ESP8266_WEB_PAGES_H_INCLUDED
