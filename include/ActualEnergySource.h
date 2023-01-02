#ifndef __ACTUALENERGYSOURCE_H__
#define __ACTUALENERGYSOURCE_H__

#include "IEnergySource.h"

#include "Common.h"

class ActualEnergySource : public virtual IEnergySource
{
public:

    ActualEnergySource(int pinVoltage, double voltageAmplitude, int pinCurrent, double currentAmplitude)
    : pinVoltage_(pinVoltage), voltageAmplitude_(voltageAmplitude), pinCurrent_(pinCurrent), currentAmplitude_(currentAmplitude)
    {
    }

    virtual ~ActualEnergySource() {}

    virtual double voltage()
    {
        return analogRead(A1) * 2 * voltageAmplitude_ / 1024 - voltageAmplitude_;
    }

    virtual double current()
    {
        return analogRead(A2) * 2 * currentAmplitude_ / 1024 - currentAmplitude_;
    }

private:

    double readTimeInSecs() {

        return ((double) micros()) / MICROSECONDS_PER_SECOND;
    }

    int pinVoltage_;
    double voltageAmplitude_;
    int pinCurrent_;
    double currentAmplitude_;
};

#endif // __ACTUALENERGYSOURCE_H__