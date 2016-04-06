#ifndef EqualizerLegMode_h
#define EqualizerLegMode_h

#include "LegMode.h"

class EqualizerLegMode : public LegMode
{
    public:
        EqualizerLegMode() : eqLevel(0.0), _direction(1) {}

        float eqLevel;
        virtual void setup(LWConfigs *c, char const *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        double _perlinsTracker;
        double _x;
        double _y;
        int _direction;
        unsigned long _lastChangeTimer;
};

#endif
