#ifndef __ACTUALENERGYSOURCE_H__
#define __ACTUALENERGYSOURCE_H__

#include "IEnergySource.h"

#include "Common.h"

#include "EnergySample.h"

struct MeasureInput
{
    MeasureInput() {}

    MeasureInput(int pin, float amplitude)
    : pin_(pin), amplitude_(amplitude)
    {}

    int pin_;
    float amplitude_;
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

    virtual int currentInputCount()
    {
        return currentInputCount_;
    }

    virtual void capture(EnergySample& energySample)
    {
        energySample.currentInputCount_ = currentInputCount_;
        energySample.voltageInputValue_ = analogRead(voltageInput_.pin_);
        for (auto inputIndex = 0; inputIndex < currentInputCount_; ++inputIndex)
        {
            energySample.currentInputValues_[inputIndex] = analogRead(currentInputs_[inputIndex].pin_);
            energySample.currentAmplitudes_[inputIndex] = currentInputs_[inputIndex].amplitude_;
        }
        energySample.voltageAmplitude_ = voltageInput_.amplitude_;
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