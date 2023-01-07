#ifndef __ENERGYMETER_H__
#define __ENERGYMETER_H__

#include "IEnergySource.h"

#include "Common.h"
#include "ElapsedTime.h"
#include "EnergySample.h"

class EnergyMeter
{

public:

    EnergyMeter(
        IEnergySource& energySource,
        float supplyVoltage,
        float samplingFrequency,
        unsigned long samplingDurationInMs,
        float calibrationVoltageMultiplier,
        float calibrationVoltageOffset,
        const float calibrationCurrentMultiplier[],
        const float calibrationCurrentOffset[],
        const float calibrationPhaseShift[],
        int currentInputCount)
    : energySource_(energySource),
    samplingFrequency_(samplingFrequency),
    samplingDurationInMs_(samplingDurationInMs),
    voltageMultiplier_(supplyVoltage * calibrationVoltageMultiplier / 2),
    voltageOffset_(calibrationVoltageOffset),
    currentMultiplier_(),
    currentOffset_(),
    calibrationPhaseShift_(),
    currentInputCount_(currentInputCount),
    frequency_(0.0),
    rmsVoltage_(0.0),    
    rmsCurrent_(),
    apparentPower_(),
    activePower_(),
    reactivePower_(),
    energy_(),
    powerFactor_(),
    phaseAngle_()
    {
        for (auto inputIndex = 0; inputIndex < currentInputCount_; ++inputIndex)
        {
            currentMultiplier_[inputIndex] = supplyVoltage * calibrationCurrentMultiplier[inputIndex] / 2;
            currentOffset_[inputIndex] = calibrationCurrentOffset[inputIndex];
        }
        memcpy(calibrationPhaseShift_, calibrationPhaseShift, currentInputCount_ * sizeof(float));
    }

    void update()
    {
        Sampling sampling;

        sample(sampling, samplingDurationInMs_);

        sampleCount_ = sampling.sampleCount;

        frequency_ = ((float) sampling.periodCount) / samplingDurationInMs_ * MILLISECONDS_PER_SECOND;
        rmsVoltage_ = voltageMultiplier_ * sqrt(sampling.voltageSqrSum / sampling.sampleCount);
        averageVoltage_ = sampling.voltageSum / sampling.sampleCount * rmsVoltage_ * sqrt(2);

        //currentInputCount_ = sampling.currentInputCount;
        for (auto inputIndex = 0; inputIndex < sampling.currentInputCount; ++inputIndex)
        {            
            rmsCurrent_[inputIndex] = currentMultiplier_[inputIndex] * sqrtf(sampling.currentSqrSum[inputIndex] / sampling.sampleCount);
            averageCurrent_[inputIndex] = sampling.currentSum[inputIndex] / sampling.sampleCount * rmsCurrent_[inputIndex] * sqrt(2);
            apparentPower_[inputIndex] = rmsVoltage_ * rmsCurrent_[inputIndex];
            activePower_[inputIndex] = voltageMultiplier_ * currentMultiplier_[inputIndex] * sampling.instantPowerSum[inputIndex] / sampling.sampleCount;
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

    /* Average voltage value in Volt (V). Used for calibration. It should be zero.*/ 
    float averageVoltage() { return averageVoltage_; }

    /* Average current value in Ampere (A). Used for calibration. It should be zero.*/ 
    float averageCurrent(int inputIndex) { return averageCurrent_[inputIndex]; }

    /* RMS voltage in Volt (V). */
    float rmsVoltage() { return rmsVoltage_; }

    /* Peak voltage in Volt (V). */
    float peakVoltage() { return rmsVoltage_ * sqrt(2); }

    /* Number of current input. */
    int currentInputCount() { return currentInputCount_; }

    /* RMS current in Ampere (A). */
    float rmsCurrent(int inputIndex) { return rmsCurrent_[inputIndex]; }

    /* Peak current in Ampere (A). */
    float peakCurrent(int inputIndex) { return rmsCurrent_[inputIndex] * sqrt(2); }

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
        float voltageSum;
        float currentSum[MAX_CURRENT_INPUT];
        float voltageSqrSum;

        int currentInputCount;
        float currentSqrSum[MAX_CURRENT_INPUT];
        float instantPowerSum[MAX_CURRENT_INPUT];

        unsigned long totalElapsed;
        unsigned long busyElapsed;
    };

    void sample(Sampling& sampling, unsigned long samplingDurationInMs) {

        sampling.sampleCount = 0;
        sampling.periodCount = 0;
        sampling.voltageSum = 0.0;
        sampling.voltageSqrSum = 0.0;

        sampling.currentInputCount = energySource_.currentInputCount();
        for (auto index = 0; index < sampling.currentInputCount; ++index)
        {
            sampling.currentSum[index] = 0.0;
            sampling.currentSqrSum[index] = 0.0;
            sampling.instantPowerSum[index] = 0.0;
        }

        sampling.totalElapsed = 0;
        sampling.busyElapsed = 0;

        auto samplingPeriod = ((float) MICROSECONDS_PER_SECOND) / samplingFrequency_;

        auto previousVoltage = 0.0;
        float previousCurrent[MAX_CURRENT_INPUT];

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

                auto voltage = energySample.voltage() + voltageOffset_;

                if (sampling.sampleCount > 0 && previousVoltage <= 0.0 && voltage > 0.0)
                {
                    ++sampling.periodCount;
                }

                sampling.voltageSum += voltage;
                sampling.voltageSqrSum += voltage * voltage;

                for (auto index = 0; index < sampling.currentInputCount; ++index)
                {
                    auto current = energySample.current(index) + currentOffset_[index];

                    float phaseAdjustedCurrent = current;
                    if (sampling.sampleCount > 0)
                    {
                        phaseAdjustedCurrent = previousCurrent[index] + calibrationPhaseShift_[index] * (current - previousCurrent[index]);
                    }

                    sampling.currentSum[index] += phaseAdjustedCurrent;
                    sampling.currentSqrSum[index] += phaseAdjustedCurrent * phaseAdjustedCurrent;
                    sampling.instantPowerSum[index] += voltage * phaseAdjustedCurrent;

                    previousCurrent[index] = current;
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
    float voltageMultiplier_;
    float voltageOffset_;
    float currentMultiplier_[MAX_CURRENT_INPUT];
    float currentOffset_[MAX_CURRENT_INPUT];
    float calibrationPhaseShift_[MAX_CURRENT_INPUT];
    int currentInputCount_;

    unsigned long sampleCount_;

    float frequency_;
    float averageVoltage_;
    float averageCurrent_[MAX_CURRENT_INPUT];
    float rmsVoltage_;


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