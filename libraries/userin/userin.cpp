
#include "Arduino.h"
#include <EEPROM.h>
#include <builtin_led.h>

namespace {
  
   // Replace with your network credentials
   constexpr size_t max_ssid = 128;
   constexpr size_t max_password = 128;

   char ssid[max_ssid] = {0};
   char password[max_password] = {0};

}

const char* get_wifi_network_ssid() { return ssid;}
const char* get_wifi_password()  { return password;}

 //  void flush_serial_input_buffer()

namespace {

   void flush_serial_input_buffer()
   {
     while (Serial.available()){
        int ch = Serial.read();
        (void)ch;
     }
   }

   constexpr char eeprom_magic[] = "bXUHBoFI2";
   constexpr size_t eeprom_magic_size = 
      sizeof(eeprom_magic)/sizeof(eeprom_magic[0]);

   int get_user_string( char* buf, size_t buf_len)
   {
       if ( (buf == nullptr) || (buf_len < 1)){
         return -1;
       }
       size_t idx = 0;
       while(idx < buf_len){

         if (Serial.available() > 0){
             int ch =Serial.read();
              if ( ch == '\r'){
                 buf[idx] = '\0';
                 return idx ;
              }else{
                 buf[idx] = ch;
                 ++idx;
              }
           
         }
       }
       Serial.println("user input too long\n");
       return 0;
   }

   bool get_eeprom_network_params()
   {
      //in ESP2866 EEPROM.begin() reads data from flash to a ram buffer.
      // After that the data is read from the buffer
      EEPROM.begin(eeprom_magic_size + max_ssid + max_password);
      char buf[eeprom_magic_size];
      for ( size_t i = 0U; i < eeprom_magic_size; ++i){
        buf[i] = EEPROM.read(i);
      }
      
      if (strncmp(buf,eeprom_magic,eeprom_magic_size) == 0){
         size_t start = eeprom_magic_size;
         bool ok = false;
         for (size_t i = 0; i < max_ssid; ++i){
           char ch = EEPROM.read(start + i);
           if ( ch == '\0'){
               if ( i > 0){
                  ssid[i] = ch;
                  ok = true;
                  break;
               }else{
                 Serial.println("invalid SSID in eeprom : STOP"); for(;;){;}
               }
           }else{
             ssid[i] = ch;
           }
         }
         if (ok == false){Serial.println("SSID too long in eeprom : STOP"); for(;;){;} }
         
         start = eeprom_magic_size + max_ssid;
         ok = false;
          for (size_t i = 0; i < max_password; ++i){
           char ch = EEPROM.read(start + i);
           if ( ch == '\0'){
               if ( i > 0){
                  password[i] = ch;
                  ok = true; 
                  break;
               }else{
                 Serial.println("invalid Pswd in eeprom : STOP"); for(;;){;}
               }
           }else{
             password[i] = ch;
           }
         }
         if (ok == false){Serial.println("bad info in eeprom : STOP"); for(;;){;} }
         //N.B. Dont use EEPROM.end() when reading
         // as it will cause a write to flash
         return true;
      }else{ // invalid magic
         return false;
      }
   }

   bool write_eeprom_network_params()
   {
       EEPROM.begin(eeprom_magic_size + max_ssid + max_password);
       size_t start = 0;
       for (size_t i = 0U; i < eeprom_magic_size; ++i){
          EEPROM.write(start + i, eeprom_magic[i]);
       }
       start += eeprom_magic_size;
       for (size_t i = 0U; i < max_ssid; ++i){
          EEPROM.write(start + i, ssid[i]);
       }
       start += max_ssid;
       for (size_t i = 0U; i < max_password; ++i){
          EEPROM.write(start + i, password[i]);
       }
       //###################################
       //N.B. Must call end to write the data to flash
       // prior it is only held in a ram cache

       EEPROM.end();
       //######################################
       return true;
   }

   bool get_manual_network_params()
   {
         for (;;){
          Serial.print("Enter ssid: ");
          int ret = get_user_string(ssid,max_ssid);
          if (ret < 0){
            Serial.print("Error from get ssid : STOP!\n"); for(;;){;}
          }
          if ( ret > 0) {
            Serial.println(ssid);
            break;
          }else {
              Serial.println("");
          }
         }
     
         for (;;){
          Serial.print("Enter passwd: ");
          int ret = get_user_string(password,max_password);
          if (ret < 0){
            Serial.print("Error from get password : STOP!\n"); for(;;){;}
          }
          if ( ret > 0) {
            Serial.println("password");
            break;
          } else {
              Serial.println("");
          }
         }
       return true;
   }

   bool do_setup()
   {
       while (! get_manual_network_params()) {;}
       flush_serial_input_buffer();
       Serial.println("Save Settings(y\\n)? ");
       while (! Serial.available()){;}
       char ch = Serial.read();
       if (( ch == 'y' ) || (ch == 'Y')){
          Serial.println("y");
          return write_eeprom_network_params();
       }else {
          Serial.println("n");
          return true;
       }
   }

   bool get_network_params()
   {
     if ( ! get_eeprom_network_params()){
         Serial.println("No settings found, Please enter manually");
         return do_setup();
     }
     return true;
   }

}  // ~namespace


void setup_network_params()
{
  auto now = millis();
  int rets = 0;
  auto last_led_switch = now;
  //delay here so that user can change network ssid and passwd if wants
  flush_serial_input_buffer();
  Serial.println("Press ret x 3 for setup");
  while ( (millis() - now) < 5000){
    if ( Serial.available() && ( Serial.read() == '\r' ) ){
       if (++rets > 2){
          if(do_setup()){
             return;
          }else{
             break;
          }
       }
    }
    if ( (millis() - last_led_switch) >= 1000){
      last_led_switch = millis();
      complement_builtin_led();
    }
  }
  // params should now be in eeprom, if not ask user
  while (! get_network_params() ) { asm volatile("nop":::);}
}
