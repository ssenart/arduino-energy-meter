#ifndef __COMMON_H__
#define __COMMON_H__

const long MICROSECONDS_PER_SECOND = 1000000;

const int SECONDS_PER_HOUR = 3600;

const int MAX_CURRENT_INPUT = 16;

#ifndef ARDUINO
#include <math.h>
#include <chrono>
#include <iostream>
#define PI 3.1415926535897932384626433832795
#define A1 1
#define A2 2
unsigned long micros(void)
{
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch();

    return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
}
int analogRead(uint8_t pin)
{
    return 0;
}
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
class SerialPrintMock
{
public:
    void begin(int bauds) { }
    void print(const char* message) { std::cout << message; }
    void print(float value) { std::cout << value; }
    void println(const char* message) { std::cout << message << std::endl; }
    void println(float value) { std::cout << value << std::endl; }
};
SerialPrintMock Serial;
#else
#include <Arduino.h>
#endif

#endif // __COMMON_H__
