#ifndef EqualizerLegMode_h
#define EqualizerLegMode_h

#include "LegMode.h"

class EqualizerLegMode : public LegMode
{
    public:
        float eqLevel = 0.0;
        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        elapsedMillis _lastChangeTimer = 0;
};

#endif
