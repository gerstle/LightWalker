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
        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        elapsedMillis _lastChangeTimer;
        LightStateEnum _state = None;

        int _lowerFootBorder;
        int _upperFootBorder;

        void _setState(LightStateEnum newState);
        void _sameStatus();
        void _footDown();
        void _flash();
        void _shimmer();
        void _sparkle();
        void _fade();
};

#endif
