#ifndef ChaosLegMode_h
#define ChaosLegMode_h

#include "LegMode.h"

class ChaosLegMode : public LegMode
{
    public:
        ChaosLegMode() : _lastStepTime(millis()), _perlinsTracker(0.0), _hue(0) {}

        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        unsigned long _currentTime;
        unsigned long _lastStepTime;
        double _perlinsTracker;
        double _x;
        double _y;
        byte _hue;
        int _direction;
        unsigned long _lastChangeTimer;
};

#endif
