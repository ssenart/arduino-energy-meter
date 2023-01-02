#ifndef __IENERGYSOURCE_H__
#define __IENERGYSOURCE_H__

class IEnergySource
{
public:

    virtual ~IEnergySource() {}

    virtual double voltage() = 0;

    virtual double current() = 0;

protected:

    IEnergySource() {}
};

#endif // __IENERGYSOURCE_H__