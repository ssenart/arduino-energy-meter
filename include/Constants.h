#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

const long MICROSECONDS_PER_SECOND = 1000000;

const int SECONDS_PER_HOUR = 3600;

#ifndef ARDUINO
#define PI 3.1415926535897932384626433832795

#include <chrono>
unsigned long micros(void)
{
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch();

    return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
}
#endif

#endif // __CONSTANTS_H__