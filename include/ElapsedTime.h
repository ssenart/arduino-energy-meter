#ifndef __ELAPSEDTIME_H__
#define __ELAPSEDTIME_H__

#include "Common.h"

#define ELAPSED_TIME(elapsed) ElapsedTime __elapsed_time__(elapsed)

class ElapsedTime
{
public:
    ElapsedTime(unsigned long& elapsed)
    : elapsed_(elapsed)
    {
        startTime_ = micros();        
    }

    virtual ~ElapsedTime()
    {
        elapsed_ += micros() - startTime_;
    }

private:
    unsigned long& elapsed_;
    unsigned long startTime_;
};

#endif // __ELAPSEDTIME_H__