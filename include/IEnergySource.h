#ifndef __IENERGYSOURCE_H__
#define __IENERGYSOURCE_H__

class EnergySample;

class IEnergySource
{
public:

    virtual ~IEnergySource() {}

    virtual int currentInputCount() = 0;

    virtual void capture(EnergySample& energySample) = 0;

protected:

    IEnergySource() {}
};

#endif // __IENERGYSOURCE_H__