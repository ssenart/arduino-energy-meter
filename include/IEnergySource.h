#ifndef __IENERGYSOURCE_H__
#define __IENERGYSOURCE_H__

class IEnergySource
{
public:

    virtual ~IEnergySource() {}

    virtual float voltage() = 0;

    virtual float current(int inputIndex) = 0;

    virtual int currentInputCount() = 0;

protected:

    IEnergySource() {}
};

#endif // __IENERGYSOURCE_H__