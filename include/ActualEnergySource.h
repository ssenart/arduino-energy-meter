#ifndef __ACTUALENERGYSOURCE_H__
#define __ACTUALENERGYSOURCE_H__

#include "IEnergySource.h"

#include "Common.h"

#include "EnergySample.h"

struct MeasureInput
{
    MeasureInput() {}

    MeasureInput(int pin, float multiplier)
    : pin_(pin), multiplier_(multiplier)
    {}

    int pin_;
    float multiplier_;
};

class ActualEnergySource : public virtual IEnergySource
{
public:

    ActualEnergySource(int adcResolution, int voltageInputPin, int currentInputPins[MAX_CURRENT_INPUT], int currentInputCount)
    : adcMaximumValue_(adcResolution<<1), voltageInputPin_(voltageInputPin), currentInputPins_(), currentInputCount_(currentInputCount)
    {
        memcpy(currentInputPins_, currentInputPins, currentInputCount_ * sizeof(int));
    }

    virtual ~ActualEnergySource() {}

    virtual int currentInputCount()
    {
        return currentInputCount_;
    }

    virtual void capture(EnergySample& energySample)
    {
        energySample.adcMaximumValue_ = adcMaximumValue_;
        energySample.currentInputCount_ = currentInputCount_;
        energySample.voltageInputValue_ = analogRead(voltageInputPin_);
        for (auto inputIndex = 0; inputIndex < currentInputCount_; ++inputIndex)
        {
            energySample.currentInputValues_[inputIndex] = analogRead(currentInputPins_[inputIndex]);
        }
    }

private:

    float readTimeInSecs() {

        return ((float) micros()) / MICROSECONDS_PER_SECOND;
    }

    int adcMaximumValue_;

    int voltageInputPin_;
    int currentInputPins_[MAX_CURRENT_INPUT];
    int currentInputCount_;
};

#endif // __ACTUALENERGYSOURCE_H__