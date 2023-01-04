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
        return voltageInputValue_ * 2 * voltageAmplitude_ / 1024 - voltageAmplitude_;
    }

    float current(int inputIndex)
    {
        return currentInputValues_[inputIndex] * 2 * currentAmplitudes_[inputIndex] / 1024 - currentAmplitudes_[inputIndex];
    }

    int currentInputCount() 
    {
        return currentInputCount_;
    }

private:

    int voltageInputValue_;
    float voltageAmplitude_;

    int currentInputCount_;
    int currentInputValues_[MAX_CURRENT_INPUT];
    float currentAmplitudes_[MAX_CURRENT_INPUT];

    friend class ActualEnergySource;
    friend class SimulatedEnergySource;
};

#endif // __ENERGYSAMPLE_H__