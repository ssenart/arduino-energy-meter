#ifndef __SIMULATEDENERGYSOURCE_H__
#define __SIMULATEDENERGYSOURCE_H__

#include "IEnergySource.h"

#include "Common.h"

#include "EnergySample.h"

#include <math.h>

class SimulatedEnergySource : public virtual IEnergySource
{
public:

    SimulatedEnergySource(int adcResolution, float frequency, float phaseAngle, int currentInputCount)
    : adcMaximumValue_(1<<adcResolution), frequency_(frequency), phaseAngle_(phaseAngle), currentInputCount_(currentInputCount)
    {
    }

    virtual ~SimulatedEnergySource() {}

    virtual int currentInputCount()
    {
        return currentInputCount_;
    }

    virtual void capture(EnergySample& energySample)
    {
        energySample.adcMaximumValue_ = adcMaximumValue_;
        energySample.currentInputCount_ = currentInputCount_;
        energySample.voltageInputValue_ = round((sinf(2 * PI * frequency_ * readTimeInSecs()) + 0.5) * adcMaximumValue_);
        for (auto inputIndex = 0; inputIndex < currentInputCount_; ++inputIndex)
        {
            energySample.currentInputValues_[inputIndex] = round((sinf(2 * PI * frequency_ * readTimeInSecs() + phaseAngle_) + 0.5) * adcMaximumValue_);
        }
    }

private:

    float readTimeInSecs() {

        return ((float) micros()) / MICROSECONDS_PER_SECOND;
    }

    int adcMaximumValue_;

    float frequency_;
    float phaseAngle_;
    int currentInputCount_;
};

#endif // __SIMULATEDENERGYSOURCE_H__
