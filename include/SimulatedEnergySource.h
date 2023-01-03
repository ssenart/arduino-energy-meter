#ifndef __SIMULATEDENERGYSOURCE_H__
#define __SIMULATEDENERGYSOURCE_H__

#include "IEnergySource.h"

#include "Common.h"

class SimulatedEnergySource : public virtual IEnergySource
{
public:

    SimulatedEnergySource(float voltageAmplitude, float currentAmplitude, float frequency, float phaseAngle, int currentInputCount)
    : voltageAmplitude_(voltageAmplitude), currentAmplitude_(currentAmplitude), frequency_(frequency), phaseAngle_(phaseAngle), currentInputCount_(currentInputCount)
    {
    }

    virtual ~SimulatedEnergySource() {}

    virtual float voltage()
    {
        return voltageAmplitude_ * sin(2 * PI * frequency_ * readTimeInSecs());
    }

    virtual float current(int inputIndex)
    {
        return currentAmplitude_ * sin(2 * PI * frequency_ * readTimeInSecs() + phaseAngle_);
    }

    virtual int currentInputCount()
    {
        return currentInputCount_;
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
