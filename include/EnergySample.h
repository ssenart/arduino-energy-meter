#ifndef __ENERGYSAMPLE_H__
#define __ENERGYSAMPLE_H__

#include "Common.h"

class EnergySample
{
public:

    EnergySample() {}

    virtual ~EnergySample() {}

    float voltage()
    {
        return ((float) voltageInputValue_) / adcMaximumValue_ - 0.5;
    }

    float current(int inputIndex)
    {
        return ((float) currentInputValues_[inputIndex]) / adcMaximumValue_ - 0.5;
    }

    int currentInputCount() 
    {
        return currentInputCount_;
    }

private:

    int adcMaximumValue_;
    int voltageInputValue_;
    int currentInputCount_;
    int currentInputValues_[MAX_CURRENT_INPUT];

    friend class ActualEnergySource;
    friend class SimulatedEnergySource;
};

#endif // __ENERGYSAMPLE_H__