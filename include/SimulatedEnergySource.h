#ifndef __SIMULATEDENERGYSOURCE_H__
#define __SIMULATEDENERGYSOURCE_H__

#include "IEnergySource.h"

#include "Constants.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

class SimulatedEnergySource : public virtual IEnergySource
{
public:

    SimulatedEnergySource(double voltageAmplitude, double currentAmplitude, double frequency, double phaseAngle)
    : voltageAmplitude_(voltageAmplitude), currentAmplitude_(currentAmplitude), frequency_(frequency), phaseAngle_(phaseAngle)
    {
    }

    virtual ~SimulatedEnergySource() {}

    virtual double voltage()
    {
        return voltageAmplitude_ * sin(2 * PI * frequency_ * readTimeInSecs());
    }

    virtual double current()
    {
        return currentAmplitude_ * sin(2 * PI * frequency_ * readTimeInSecs() + phaseAngle_);
    }

private:

    double readTimeInSecs() {

        return ((double) micros()) / MICROSECONDS_PER_SECOND;
    }

    double voltageAmplitude_;
    double currentAmplitude_;
    double frequency_;
    double phaseAngle_;
};

#endif // __SIMULATEDENERGYSOURCE_H__
