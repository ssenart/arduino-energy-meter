#ifndef __SIMULATEDENERGYSOURCE_H__
#define __SIMULATEDENERGYSOURCE_H__

#include "IEnergySource.h"

#include "Common.h"

#include "EnergySample.h"

#include <math.h>

class SimulatedEnergySource : public virtual IEnergySource
{
public:

    SimulatedEnergySource(float voltageAmplitude, float currentAmplitude, float frequency, float phaseAngle, int currentInputCount)
    : voltageAmplitude_(voltageAmplitude), currentAmplitude_(currentAmplitude), frequency_(frequency), phaseAngle_(phaseAngle), currentInputCount_(currentInputCount)
    {
    }

    virtual ~SimulatedEnergySource() {}

    virtual int currentInputCount()
    {
        return currentInputCount_;
    }

    virtual void capture(EnergySample& energySample)
    {
        energySample.currentInputCount_ = currentInputCount_;
        energySample.voltageInputValue_ = round(sinf(2 * PI * frequency_ * readTimeInSecs()) * 512 + 512);
        for (auto inputIndex = 0; inputIndex < currentInputCount_; ++inputIndex)
        {
            energySample.currentInputValues_[inputIndex] = round(sinf(2 * PI * frequency_ * readTimeInSecs() + phaseAngle_) * 512 + 512);
            energySample.currentAmplitudes_[inputIndex] = currentAmplitude_;
        }
        energySample.voltageAmplitude_ = voltageAmplitude_;
    }

private:

    float readTimeInSecs() {

        return ((float) micros()) / MICROSECONDS_PER_SECOND;
    }

    float voltageAmplitude_;
    float currentAmplitude_;
    float frequency_;
    float phaseAngle_;
    int currentInputCount_;
};

#endif // __SIMULATEDENERGYSOURCE_H__
