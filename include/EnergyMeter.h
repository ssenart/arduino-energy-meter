#ifndef __ENERGYMETER_H__
#define __ENERGYMETER_H__

#include "IEnergySource.h"

#include "Constants.h"

#include <Arduino.h>

class EnergyMeter
{

public:

    EnergyMeter(IEnergySource& energySource, double samplingFrequency, int sampleSize)
    : energySource_(energySource), samplingFrequency_(samplingFrequency), sampleSize_(sampleSize)
    {
    }

    void update()
    {
        Sampling sampling;

        sample(sampling, sampleSize_);

        frequency_ = ((double) sampling.periodCount) / (((double) sampleSize_) / samplingFrequency_);
        rmsVoltage_ = sqrt(sampling.voltageSqrSum / sampling.size);
        rmsCurrent_ = sqrt(sampling.currentSqrSum / sampling.size);
        apparentPower_ = rmsVoltage_ * rmsCurrent_;
        activePower_ = sampling.instantPowerSum / sampling.size;
        reactivePower_ = sqrt(sqr(apparentPower_) - sqr(activePower_));
        powerFactor_ = activePower_ / apparentPower_;
        phaseAngle_  = acos(powerFactor_) * 180 / PI;
    }

    double frequency() { return frequency_; }

    double rmsVoltage() { return rmsVoltage_; }

    double rmsCurrent() { return rmsCurrent_; }

    double apparentPower() { return apparentPower_; }

    double activePower() { return activePower_; }

    double reactivePower() { return reactivePower_; }

    double powerFactor() { return powerFactor_; }

    double phaseAngle() { return phaseAngle_; }    

private:

    struct Sampling
    {
        int size;
        int periodCount;
        double voltageSqrSum;
        double currentSqrSum;
        double instantPowerSum;
    };

    double sqr(double val)
    {
        return val * val;
    }

    void sample(Sampling& sampling, int size) {

        sampling.size = size;
        sampling.periodCount = 0;
        sampling.voltageSqrSum = 0;
        sampling.currentSqrSum = 0;
        sampling.instantPowerSum = 0;

        auto samplingPeriod = MICROSECONDS_PER_SECOND / samplingFrequency_;

        auto previousVoltage = 0.0;

        unsigned long previousTime = 0;

        auto sampleCount = 0;

        while (sampleCount < size) {

            auto currentTime = micros();

            if (currentTime - previousTime >= samplingPeriod)
            {
                auto voltage = energySource_.voltage();
                auto current = energySource_.current();

                if (sampleCount > 0 && previousVoltage <= 0.0 && voltage > 0.0)
                {
                    ++sampling.periodCount;
                }

                sampling.voltageSqrSum += sqr(voltage);
                sampling.currentSqrSum += sqr(current);
                sampling.instantPowerSum += voltage * current;

                previousVoltage = voltage;

                previousTime = currentTime;

                ++sampleCount;
            }
        }
    }

    IEnergySource& energySource_;
    double samplingFrequency_;
    double sampleSize_;

    double frequency_;
    double rmsVoltage_;
    double rmsCurrent_;
    double apparentPower_;
    double activePower_;
    double reactivePower_;
    double powerFactor_;
    double phaseAngle_;
};

#endif // __ENERGYMETER_H__