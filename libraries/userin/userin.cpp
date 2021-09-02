
#include "Arduino.h"
#include <EEPROM.h>
#include <builtin_led.h>
#include "userin.h"

namespace {
  
   // Replace with your network credentials
   constexpr size_t max_ssid = 128;
   constexpr size_t max_password = 128;

   char ssid[max_ssid] = {0};
   char password[max_password] = {0};

}

const char* get_wifi_network_ssid() { return ssid;}
const char* get_wifi_password()  { return password;}

namespace {

   void flush_serial_input_buffer()
   {
     while (debugSerial.available()){
        int ch = debugSerial.read();
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

         if (debugSerial.available() > 0){
             int ch =debugSerial.read();
              if ( ch == '\r'){
                 buf[idx] = '\0';
                 return idx ;
              }else{
                 buf[idx] = ch;
                 ++idx;
              }
           
         }
       }
       debugSerial.println("user input too long\n");
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
                 debugSerial.println("invalid SSID in eeprom : STOP"); for(;;){;}
               }
           }else{
             ssid[i] = ch;
           }
         }
         if (ok == false){debugSerial.println("SSID too long in eeprom : STOP"); for(;;){;} }
         
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
                 debugSerial.println("invalid Pswd in eeprom : STOP"); for(;;){;}
               }
           }else{
             password[i] = ch;
           }
         }
         if (ok == false){debugSerial.println("bad info in eeprom : STOP"); for(;;){;} }
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
          debugSerial.print("Enter ssid: ");
          int ret = get_user_string(ssid,max_ssid);
          if (ret < 0){
            debugSerial.print("Error from get ssid : STOP!\n"); for(;;){;}
          }
          if ( ret > 0) {
            debugSerial.println(ssid);
            break;
          }else {
              debugSerial.println("");
          }
         }
     
         for (;;){
          debugSerial.print("Enter passwd: ");
          int ret = get_user_string(password,max_password);
          if (ret < 0){
            debugSerial.print("Error from get password : STOP!\n"); for(;;){;}
          }
          if ( ret > 0) {
            debugSerial.println("password");
            break;
          } else {
              debugSerial.println("");
          }
         }
       return true;
   }

   bool do_setup()
   {
       while (! get_manual_network_params()) {;}
       flush_serial_input_buffer();
       debugSerial.println("Save Settings(y\\n)? ");
       while (! debugSerial.available()){;}
       char ch = debugSerial.read();
       if (( ch == 'y' ) || (ch == 'Y')){
          debugSerial.println("y");
          return write_eeprom_network_params();
       }else {
          debugSerial.println("n");
          return true;
       }
   }

   bool get_network_params()
   {
     if ( ! get_eeprom_network_params()){
         debugSerial.println("No settings found, Please enter manually");
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
  debugSerial.println("Press ret x 3 for setup");
  while ( (millis() - now) < 5000){
    if ( debugSerial.available() && ( debugSerial.read() == '\r' ) ){
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
