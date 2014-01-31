#ifndef GravityLegMode_h
#define GravityLegMode_h

#include "LegMode.h"

class GravityLegMode : public LegMode
{
    public:
        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        elapsedMillis _lastChangeTimer;

        byte _indexOne = 0;
        byte _indexTwo = 1;
        byte _indexThree = 2;
        elapsedMillis _lastXSwitch = 0;
        elapsedMillis _lastYSwitch = 0;
        bool _canXSwitch = true;
        bool _canYSwitch = true;

};

#endif
