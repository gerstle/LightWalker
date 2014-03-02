#ifndef EqualizerLegMode_h
#define EqualizerLegMode_h

#include "LegMode.h"

class EqualizerLegMode : public LegMode
{
    public:
        EqualizerLegMode() : eqLevel(0.0), _lastChangeTimer(millis()) {}

        float eqLevel;
        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        unsigned long _lastChangeTimer;
};

#endif
