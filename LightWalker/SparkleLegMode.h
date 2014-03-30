#ifndef SparkleLegMode_h
#define SparkleLegMode_h

#include "LegMode.h"

enum LightStateEnum
{
    None,
    Flash,
    SparkleSparkle,
    Fade,
    Off
};

class SparkleLegMode : public LegMode
{
    public:
        SparkleLegMode() : _state(None) {}

        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        LightStateEnum _state;
        unsigned long _currentTime;

        int _lowerFootBorder;
        int _upperFootBorder;

        void _setState(LightStateEnum newState);
        void _sameStatus();
        void _flash();
        void _shimmer();
        void _sparkle();
        void _fade();
};

#endif
