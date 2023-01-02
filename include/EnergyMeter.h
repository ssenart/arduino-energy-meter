#ifndef __ENERGYMETER_H__
#define __ENERGYMETER_H__

#include "IEnergySource.h"

#include "Common.h"

class EnergyMeter
{

public:

    EnergyMeter(IEnergySource& energySource, double samplingFrequency, int sampleSize)
    : energySource_(energySource), samplingFrequency_(samplingFrequency), sampleSize_(sampleSize),
    frequency_(0),
    rmsVoltage_(0),
    rmsCurrent_(0),
    apparentPower_(0),
    activePower_(0),
    reactivePower_(0),
    energy_(0),
    powerFactor_(0),
    phaseAngle_(0)
    {
    }

    void update()
    {
        Sampling sampling;

        sample(sampling, sampleSize_);

        auto averagingPeriod = (((double) sampleSize_) / samplingFrequency_);

        frequency_ = ((double) sampling.periodCount) / averagingPeriod;
        rmsVoltage_ = sqrt(sampling.voltageSqrSum / sampling.size);
        rmsCurrent_ = sqrt(sampling.currentSqrSum / sampling.size);
        apparentPower_ = rmsVoltage_ * rmsCurrent_;
        activePower_ = sampling.instantPowerSum / sampling.size;
        reactivePower_ = sqrt(sqr(apparentPower_) - sqr(activePower_));
        energy_ += activePower_ * (averagingPeriod / SECONDS_PER_HOUR);
        powerFactor_ = activePower_ / apparentPower_;
        phaseAngle_  = acos(powerFactor_) * 180 / PI;
    }

    /* Frequency in Hertz (Hz). */ 
    double frequency() { return frequency_; }

    /* RMS voltage in Volt (V). */
    double rmsVoltage() { return rmsVoltage_; }

    /* RMS current in Ampere (A). */
    double rmsCurrent() { return rmsCurrent_; }

    /* Apparent power in Volt-Ampere (VA). */
    double apparentPower() { return apparentPower_; }

    /* Active power in Watt (W). */
    double activePower() { return activePower_; }

    /* Reactive power in Volt-Ampere-Reactive (VAR). */
    double reactivePower() { return reactivePower_; }

    /* Energy in Watt-Hour (Wh). */
    double energy() { return energy_; }

    /* Power factor = Cos(Phi). */
    double powerFactor() { return powerFactor_; }

    /* Phase angle (Phi) in Degree (°). */
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

        auto samplingPeriod = ((double) MICROSECONDS_PER_SECOND) / samplingFrequency_;

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
    double energy_;
    double powerFactor_;
    double phaseAngle_;
};

#endif // __ENERGYMETER_H__