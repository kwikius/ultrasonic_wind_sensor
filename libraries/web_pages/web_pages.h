#ifndef ARDUINO_ESP8266_WEB_PAGES_H_INCLUDED
#define ARDUINO_ESP8266_WEB_PAGES_H_INCLUDED

#include <Arduino.h>
#include <ESP8266WiFi.h>

void serve_page(WiFiClient & client, const char* mime_type, const char* text);

const char* get_web_page_compass_js();

const char* get_web_page_main_html();

#endif //ARDUINO_ESP8266_WEB_PAGES_H_INCLUDED
