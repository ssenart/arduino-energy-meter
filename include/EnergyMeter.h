#ifndef __ENERGYMETER_H__
#define __ENERGYMETER_H__

#include "IEnergySource.h"

#include "Common.h"
#include "ElapsedTime.h"
#include "EnergySample.h"

class EnergyMeter
{

public:

    EnergyMeter(IEnergySource& energySource, float samplingFrequency, unsigned long samplingDurationInMs)
    : energySource_(energySource), samplingFrequency_(samplingFrequency), samplingDurationInMs_(samplingDurationInMs),
    frequency_(0.0),
    rmsVoltage_(0.0),
    currentInputCount_(0),
    rmsCurrent_(),
    apparentPower_(),
    activePower_(),
    reactivePower_(),
    energy_(),
    powerFactor_(),
    phaseAngle_()
    {
    }

    void update()
    {
        Sampling sampling;

        sample(sampling, samplingDurationInMs_);

        sampleCount_ = sampling.sampleCount;

        frequency_ = ((float) sampling.periodCount) / samplingDurationInMs_ * MILLISECONDS_PER_SECOND;
        rmsVoltage_ = sqrt(sampling.voltageSqrSum / sampling.sampleCount);

        currentInputCount_ = sampling.currentInputCount;
        for (auto inputIndex = 0; inputIndex < sampling.currentInputCount; ++inputIndex)
        {
            rmsCurrent_[inputIndex] = sqrtf(sampling.currentSqrSum[inputIndex] / sampling.sampleCount);
            apparentPower_[inputIndex] = rmsVoltage_ * rmsCurrent_[inputIndex];
            activePower_[inputIndex] = sampling.instantPowerSum[inputIndex] / sampling.sampleCount;
            reactivePower_[inputIndex] = sqrtf(max(0.0, sqr(apparentPower_[inputIndex]) - sqr(activePower_[inputIndex])));
            energy_[inputIndex] += activePower_[inputIndex] * (((float) samplingDurationInMs_) / MILLISECONDS_PER_HOUR);
            powerFactor_[inputIndex] = max(-1.0, min(1.0, activePower_[inputIndex] / apparentPower_[inputIndex]));
            phaseAngle_[inputIndex]  = acosf(powerFactor_[inputIndex]) * 180 / PI;
        }

        totalElapsed_ = sampling.totalElapsed;
        busyElapsed_ = sampling.busyElapsed;
    }

    /* Number of samples captured during sampling. */
    unsigned long sampleCount() { return sampleCount_; }

    /* Frequency in Hertz (Hz). */ 
    float frequency() { return frequency_; }

    /* RMS voltage in Volt (V). */
    float rmsVoltage() { return rmsVoltage_; }

    /* Number of current input. */
    int currentInputCount() { return currentInputCount_; }

    /* RMS current in Ampere (A). */
    float rmsCurrent(int inputIndex) { return rmsCurrent_[inputIndex]; }

    /* Apparent power in Volt-Ampere (VA). */
    float apparentPower(int inputIndex) { return apparentPower_[inputIndex]; }

    /* Active power in Watt (W). */
    float activePower(int inputIndex) { return activePower_[inputIndex]; }

    /* Reactive power in Volt-Ampere-Reactive (VAR). */
    float reactivePower(int inputIndex) { return reactivePower_[inputIndex]; }

    /* Energy in Watt-Hour (Wh). */
    float energy(int inputIndex) { return energy_[inputIndex]; }

    /* Power factor = Cos(Phi). */
    float powerFactor(int inputIndex) { return powerFactor_[inputIndex]; }

    /* Phase angle (Phi) in Degree (°). */
    float phaseAngle(int inputIndex) { return phaseAngle_[inputIndex]; }    

    /* Total elapsed time in Microseconds (us). */
    float totalElapsed() { return totalElapsed_; } 

    /* Busy elapsed time in Microseconds (us). */
    float busyElapsed() { return busyElapsed_; }     

private:

    struct Sampling
    {
        unsigned long sampleCount;
        unsigned long periodCount;
        float voltageSqrSum;

        int currentInputCount;
        float currentSqrSum[MAX_CURRENT_INPUT];
        float instantPowerSum[MAX_CURRENT_INPUT];

        unsigned long totalElapsed;
        unsigned long busyElapsed;
    };

    float sqr(float val)
    {
        return val * val;
    }

    void sample(Sampling& sampling, unsigned long samplingDurationInMs) {

        sampling.sampleCount = 0;
        sampling.periodCount = 0;
        sampling.voltageSqrSum = 0.0;

        sampling.currentInputCount = energySource_.currentInputCount();
        for (auto index = 0; index < sampling.currentInputCount; ++index)
        {
            sampling.currentSqrSum[index] = 0.0;
            sampling.instantPowerSum[index] = 0.0;
        }

        sampling.totalElapsed = 0;
        sampling.busyElapsed = 0;

        auto samplingPeriod = ((float) MICROSECONDS_PER_SECOND) / samplingFrequency_;

        auto previousVoltage = 0.0;

        unsigned long previousTime = 0;

        auto stopTime = micros() + samplingDurationInMs * MICROSECONDS_PER_MILLISECOND;

        auto now = micros();

        while (now < stopTime) {

            ELAPSED_TIME(sampling.totalElapsed);

            if (now - previousTime >= samplingPeriod)
            {
                ELAPSED_TIME(sampling.busyElapsed);

                EnergySample energySample;

                energySource_.capture(energySample);

                auto voltage = energySample.voltage();

                if (sampling.sampleCount > 0 && previousVoltage <= 0.0 && voltage > 0.0)
                {
                    ++sampling.periodCount;
                }

                sampling.voltageSqrSum += sqr(voltage);

                for (auto index = 0; index < sampling.currentInputCount; ++index)
                {
                    auto current = energySample.current(index);

                    sampling.currentSqrSum[index] += sqr(current);
                    sampling.instantPowerSum[index] += voltage * current;
                }

                previousVoltage = voltage;

                ++sampling.sampleCount;

                previousTime = now;
            }

            now = micros();
        }
    }

    IEnergySource& energySource_;
    float samplingFrequency_;
    unsigned long samplingDurationInMs_;

    unsigned long sampleCount_;

    float frequency_;
    float rmsVoltage_;

    int currentInputCount_;
    float rmsCurrent_[MAX_CURRENT_INPUT];
    float apparentPower_[MAX_CURRENT_INPUT];
    float activePower_[MAX_CURRENT_INPUT];
    float reactivePower_[MAX_CURRENT_INPUT];
    float energy_[MAX_CURRENT_INPUT];
    float powerFactor_[MAX_CURRENT_INPUT];
    float phaseAngle_[MAX_CURRENT_INPUT];

    unsigned long totalElapsed_;
    unsigned long busyElapsed_;    
};

#endif // __ENERGYMETER_H__