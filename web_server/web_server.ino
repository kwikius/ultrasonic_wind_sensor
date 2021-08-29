
#include <stdlib.h>

#if defined ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>   
#else
#error need to define WiFi for this processor
#endif
#include <WebSocketsServer.h> 
  
#include <userin.h>
#include <builtin_led.h>
#include <web_pages.h>
#include <quan.h>

#include <decode_packet.hpp>
#include "windsensor_packet.hpp"

namespace {
   uint16_t constexpr basePort = 80;
   WiFiServer server(basePort);
   WebSocketsServer webSocket = WebSocketsServer(basePort + 1);
}

void webSocketEvent(byte /*num*/, WStype_t /*type*/, uint8_t * /*payload*/, size_t /*length*/)
{
  // todo upload data to server
}

const char* get_compass_js();  

void setup() 
{
  Serial.begin(115200);

  builtin_led_setup();

 // N.B.  no spaces or underscores in hostname
  WiFi.hostname("USWS-Server");

  setup_network_params();

  WiFi.begin(get_wifi_network_ssid(), get_wifi_password());

  Serial.print("Connecting ...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    complement_builtin_led();
    Serial.print(".");
  }
  turn_off_builtin_led();
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());       
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         
  const char web_page_name[] = "windsensor";

  if (!MDNS.begin(web_page_name)) { 
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("MDNS responder started");
  MDNS.addService("http", "tcp", basePort); 
  Serial.print("Web page available as ");
  Serial.print(web_page_name);
  Serial.println(".local\n");
 
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

namespace{

   void send_not_found_response(WiFiClient & client)
   {
      client.println("HTTP/1.1 404 Not Found");
      client.println("Connection: close");
      client.println();
   }

   struct uri_service{
      const String uri;
      void (*service)(WiFiClient & client);
   };

   uri_service services[] = {
      { "/", 
        [] (WiFiClient & client) { 
           serve_page(client,"text/html",get_web_page_main_html());
         }
      },
      { "/compass.js", 
        [] (WiFiClient & client) { 
           serve_page(client,"text/javascript",get_web_page_compass_js());
         }
      }
   };

   bool service(WiFiClient & client, String const & uri)
   {
      for (auto const & srv : services){
         if ( uri == srv.uri){
            srv.service(client);
            return true;
         }
      }
      return false;
   }

   void processGET(WiFiClient & client)
   {
      if (client) {                             
         Serial.println("New Client.");            
         String request;          
         bool newline = false;
         while (client.connected()) {           
            if (client.available()) {             
               char const c = client.read();             
               Serial.write(c);                    
               request += c;
               String filename;
               switch (c){
                  case '\r':
                     break; // ignore
                  case '\n':
                     if (newline){
                        // 2 X newline so end of header. Process the header
                        bool serviced = false;
                        int const get_pos = request.indexOf("GET /");
                        if (get_pos >= 0){
                           int const end_address = request.indexOf(" ",get_pos + 4);
                           if ( end_address >= 0){
                              filename = request.substring(get_pos + 4,end_address);
                              serviced = service(client,filename);
                           }
                        }
                        if (!serviced){
                           Serial.print(filename);
                           Serial.println(" not found\n");
                           send_not_found_response(client);
                        }
                        client.println();
                        client.stop();
                     }else{
                        newline = true;
                     }
                     break;
                  default:
                     newline = false;
                     break;
               } //~switch
            }// ~if
         }//~while
         Serial.println("Client disconnected.");
         Serial.println("");
      }//~if
   }

   cobs::static_packet_parser<
      cobs::get_encoded_length<sizeof(velocity_and_direction_packet)>()
   > parser;

   void parse_input()
   {
      while(Serial.available() ){ 
         uint8_t const ch = Serial.read();
         uint16_t const packet_length = parser.parse(ch);
         if ( packet_length > 0U ){
            uint8_t const * decoded_packet_buffer = parser.get_decoded_packet();
            uint16_t const packet_id = static_cast<uint16_t>(decoded_packet_buffer[0]) 
            + (static_cast<uint16_t>(decoded_packet_buffer[1]) << 8U);
            if ( static_cast<PacketID>(packet_id) == PacketID::WindSpeedAndDirection){
               if ( !zapp4::decode_packet<velocity_and_direction_packet>(
                     packet_length, parser,
                     [](velocity_and_direction_packet const & p ){
                        String const text = "{\"windspeed\":" + String(p.values.speed.numeric_value(),2) + 
                        ", \"windangle\":" + String(p.values.direction.numeric_value(),1) + "}";
                        webSocket.broadcastTXT(text.c_str());
                     }
                  )
               ) {
                  Serial.println("parse packet failed");
               }
            }
         }
      }
   }
} // ~namespace

void loop(){

   MDNS.update();

   parse_input();

   webSocket.loop();

   WiFiClient client = server.available();
   if ( client){ 
      processGET(client);
   }
}
