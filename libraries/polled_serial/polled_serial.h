#ifndef ULTRASONIC_SENSOR_POLLED_SERIAL_H_INCLUDED
#define ULTRASONIC_SENSOR_POLLED_SERIAL_H_INCLUDED

#include <HardwareSerial.h>

/**
  Arduino serial output without using uart interrupts.
**/

class PolledSerial : public Stream{

  public:
    PolledSerial(): m_rx_buf_full{false}{}
    void begin(unsigned long baud) { begin(baud, SERIAL_8N1); }
    void begin(unsigned long baud, uint8_t config);
    void end();
    int available();
    int peek();
    int read();
    //int availableForWrite() { return 0;} default
    void flush();
    size_t write(uint8_t);
   // size_t write(const uint8_t *buffer, size_t size);
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool()const { return true; }

    private:
     bool m_rx_buf_full;
     uint8_t m_rx_buf;
};

extern PolledSerial polledSerial;

#endif // ULTRASONIC_SENSOR_POLLED_SERIAL_H_INCLUDED
