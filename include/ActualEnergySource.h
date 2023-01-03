#ifndef __ACTUALENERGYSOURCE_H__
#define __ACTUALENERGYSOURCE_H__

#include "IEnergySource.h"

#include "Common.h"

struct MeasureInput
{
    int pin;
    float amplitude;
};

class ActualEnergySource : public virtual IEnergySource
{
public:

    ActualEnergySource(const MeasureInput& voltageInput, const MeasureInput currentInputs[MAX_CURRENT_INPUT], int currentInputCount)
    : voltageInput_(voltageInput), currentInputs_(), currentInputCount_(currentInputCount)
    {
        memcpy(currentInputs_, currentInputs, currentInputCount_);
    }

    virtual ~ActualEnergySource() {}

    virtual float voltage()
    {
        return analogRead(voltageInput_.pin) * 2 * voltageInput_.amplitude / 1024 - voltageInput_.amplitude;
    }

    virtual float current(int inputIndex)
    {
        return analogRead(currentInputs_[inputIndex].pin) * 2 * currentInputs_[inputIndex].amplitude / 1024 - currentInputs_[inputIndex].amplitude;
    }

    virtual int currentInputCount()
    {
        return currentInputCount_;
    }

private:

    float readTimeInSecs() {

        return ((float) micros()) / MICROSECONDS_PER_SECOND;
    }

    MeasureInput voltageInput_;
    MeasureInput currentInputs_[MAX_CURRENT_INPUT];
    int currentInputCount_;
};

#endif // __ACTUALENERGYSOURCE_H__