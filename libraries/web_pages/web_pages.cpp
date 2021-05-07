
#include "web_pages.h"

namespace {
   
   // fill str with firsy line if any
   // return one past return or null for end
   const char* getline( const char* char_str, String & str)
   {
      for(;;){
         char ch = *char_str;
         if ( ch == '\0'){
            return char_str;
         } else if ( ch == '\n'){
            return char_str + 1;
         } else{
            str += ch;
            ++ char_str;
         }
      }   
   }
   
}

void serve_page(WiFiClient & client,const char* text)
{
   while(*text){
      String str;
      text = getline(text,str);
      client.println(str);
   }
}
