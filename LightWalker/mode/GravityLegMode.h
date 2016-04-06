#ifndef GravityLegMode_h
#define GravityLegMode_h

#include "LegMode.h"

class GravityLegMode : public LegMode
{
    public:
        GravityLegMode() : _indexOne(0), _indexTwo(1), _indexThree(2), _lastXSwitch(millis()), _lastYSwitch(millis()), _canXSwitch(true), _canYSwitch(true) {}

        virtual void setup(LWConfigs *c, char const *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        byte _indexOne;
        byte _indexTwo;
        byte _indexThree;
        unsigned long _lastXSwitch;
        unsigned long _lastYSwitch;
        bool _canXSwitch;
        bool _canYSwitch;

};

#endif
