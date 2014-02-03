#ifndef RainbowLegMode_h
#define RainbowLegMode_h

#include "LegMode.h"

class RainbowLegMode : public LegMode
{
    public:
        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        elapsedMillis _lastChangeTimer;
        byte _lastStartHue;
        float _increment;
        

        void _singleRainbow();
        void _doubleRainbow();
        void _rotate();
        void _rise();
        int _value;
};

#endif
