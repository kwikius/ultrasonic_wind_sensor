

#include "polled_serial.h"

PolledSerial polledSerial;

// based on HardwareSerial
void PolledSerial::begin(unsigned long baud, uint8_t config)
{
  // Try u2x mode first
  uint16_t baud_setting = (F_CPU / 4 / baud - 1) / 2;
  UCSR0A = (0b1 << U2X0);

  // hardcoded exception for 57600 for compatibility with the bootloader
  // shipped with the Duemilanove and previous boards and the firmware
  // on the 8U2 on the Uno and Mega 2560. Also, The baud_setting cannot
  // be > 4095, so switch back to non-u2x mode if the baud rate is too
  // low.
  if (((F_CPU == 16000000UL) && (baud == 57600)) || (baud_setting >4095))
  {
    UCSR0A = 0;
    baud_setting = (F_CPU / 8 / baud - 1) / 2;
  }

  // assign the baud_setting, a.k.a. ubrr (USART Baud Rate Register)
  UBRR0H = static_cast<uint8_t>((baud_setting >> 8U ) & 0xFF);
  UBRR0H = static_cast<uint8_t>(baud_setting & 0xFF);

  UCSR0C = config;

  UCSR0B |= (0b1 << RXEN0) | (0b1 << TXEN0);
}

void PolledSerial::flush(void)
{
  // wait for tx reg empty
   while( !(UCSR0A & (0b1 << UDRE0)) ){
       asm volatile ("nop":::);
   }
}

int PolledSerial::peek()
{
   if ( m_rx_buf_full){
      return m_rx_buf;
   }else{
      if ( available()){
         m_rx_buf = UDR0;
         m_rx_buf_full = true;
         return m_rx_buf;
      }else{
         return -1;
      }
   }
}

size_t PolledSerial::write(uint8_t value)
{
   flush();
   UDR0 = value;
   UCSR0A |= (1 << U2X0) | (1 << TXC0);
   return 1U;
}

int PolledSerial::read()
{
    if ( m_rx_buf_full){
      m_rx_buf_full = false;
      return m_rx_buf;
   }else{
      if ( available()){
         return UDR0;
      }else{
         return -1;
      }
   }
}

int PolledSerial::available()
{
   if ( m_rx_buf_full){
      return 1;
   }else{
      return ((UCSR0A & ( 0b1 << RXC0)) != 0)? 1 : 0;
   }
}

void PolledSerial::end()
{
  flush();
  UCSR0B &= ~((0b1 << RXEN0) | (0b1 << TXEN0));
}
