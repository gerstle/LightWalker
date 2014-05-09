#ifndef RainbowLegMode_h
#define RainbowLegMode_h

#include "LegMode.h"

class RainbowLegMode : public LegMode
{
    public:
        RainbowLegMode() : _lastStepTime(millis()), _lastStartHue(0), _increment(0), _perlinZ(0.0) {}

        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        unsigned long _currentTime;
        unsigned long _lastStepTime;
        byte _lastStartHue;
        float _increment;
        
        void _singleRainbow();
        void _doubleRainbow();
        void _rotate();
        void _rise();
        byte _getValue(int i);

        double _perlinZ;
        byte _halfMin;
        unsigned long _lastNoiseTimer;
};

#endif
