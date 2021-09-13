
#include <stdlib.h>

#if defined ESP8266
   #include <ESP8266WiFi.h>
   #include <ESP8266mDNS.h>   
#else
   #if defined ESP32
      #include <WiFi.h>
      #include <ESPmDNS.h>
   #else
      #error unknown processor
   #endif
#endif

#include <WebSocketsServer.h> 
  
#include <userin.h>
#include <builtin_led.h>
#include <web_pages.h>
#include <quan.h>

#include <windsensor_packet.h>

#if defined ESP8266
   HardwareSerial& debugSerial = Serial;
   HardwareSerial& sensorSerial = Serial;
#else
   #if defined ESP32
      HardwareSerial& debugSerial = Serial;
      HardwareSerial& sensorSerial = Serial2;
   #else
      #error need to define serial ports 
   #endif
#endif

namespace {

   uint16_t constexpr basePort = 80;

   // N.B.  no spaces or underscores in hostname
   constexpr char wifiHostName[] = "USWS-Server";
   constexpr char mdns_local_name[] = "windsensor";

   void webSocketEvent(byte /*num*/, WStype_t /*type*/, uint8_t * /*payload*/, size_t /*length*/)
   {
     // todo upload data to server
   }

   WiFiServer server(basePort);
   WebSocketsServer webSocket = WebSocketsServer(basePort + 1);

   void WifiConnect()
   {
 
     WiFi.hostname(wifiHostName);

     setup_network_params();

     WiFi.begin(get_wifi_network_ssid(), get_wifi_password());
     debugSerial.print("Connecting ...");
     while (WiFi.status() != WL_CONNECTED) {
       delay(250);
       complement_builtin_led();
       debugSerial.print(".");
     }

     turn_off_builtin_led();
     debugSerial.println('\n');
     debugSerial.print("Connected to : ");
     debugSerial.println(WiFi.SSID());       
     debugSerial.print("IP address   : ");
     debugSerial.println(WiFi.localIP());         

     if (!MDNS.begin(mdns_local_name)) { 
        debugSerial.println("Error setting up MDNS responder!");
     }
     debugSerial.println("MDNS responder started");
     MDNS.addService("http", "tcp", basePort); 
     debugSerial.print("Web page available as ");
     debugSerial.print(mdns_local_name);
     debugSerial.println(".local\n");
    
     server.begin();
     webSocket.begin();
     webSocket.onEvent(webSocketEvent);
   }
}

void setup() 
{
   debugSerial.begin(115200);
   // debugSerial and sensorSerial may be 2 separate ports
   if ( &debugSerial != &sensorSerial){
      sensorSerial.begin(115200);
   }
   builtin_led_setup();

   WifiConnect();
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
         debugSerial.println("New Client.");            
         String request;          
         bool newline = false;
         while (client.connected()) {           
            if (client.available()) {             
               char const c = client.read();             
               debugSerial.write(c);                    
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
                           debugSerial.print(filename);
                           debugSerial.println(" not found\n");
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
         debugSerial.println("Client disconnected.");
         debugSerial.println("");
      }//~if
   }

   quan::uav::cobs::static_packet_parser<
      quan::uav::cobs::get_encoded_length<sizeof(velocity_and_direction_packet)>()
   > parser;

   void parse_input()
   {
      while(sensorSerial.available() ){ 
         uint8_t const ch = sensorSerial.read();
         uint16_t const packet_length = parser.parse(ch);
         if ( packet_length > 0U ){
            uint8_t const * decoded_packet_buffer = parser.get_decoded_packet();
            uint16_t const packet_id = static_cast<uint16_t>(decoded_packet_buffer[0]) 
            + (static_cast<uint16_t>(decoded_packet_buffer[1]) << 8U);
            if ( static_cast<PacketID>(packet_id) == PacketID::WindSpeedAndDirection){
               if ( !quan::tracker::zapp4::decode_packet<velocity_and_direction_packet>(
                     packet_length, parser,
                     [](velocity_and_direction_packet const & p ){
                        String const text = "{\"windspeed\":" + String(p.values.speed.numeric_value(),2) + 
                        ", \"windangle\":" + String(p.values.direction.numeric_value(),1) + "}";
                        webSocket.broadcastTXT(text.c_str());
                     }
                  )
               ) {
                  debugSerial.println("parse packet failed");
               }
            }
         }
      }
   }
} // ~namespace

void loop(){

#if defined ESP8266
   MDNS.update();
#endif
   parse_input();

   webSocket.loop();

   WiFiClient client = server.available();
   if ( client){ 
      processGET(client);
   }
}
