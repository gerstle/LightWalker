#ifndef LegMode_h
#define LegMode_h

#include "LWConfigs.h"
#include "ADXL345_compatible.h"

class LegMode
{
    public:
        LegMode() : stepDetected(false), _lastChangeTimer(millis()), _setup_complete(false) {}

        bool stepDetected;

        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p) = 0;
        virtual void frame() = 0;

    protected:
        unsigned long _lastChangeTimer;
        bool _setup_complete;

        LWConfigs *_config;
        char *_legName;
        byte _channel;
        ADXL345 *_adxl;
        byte _pixelCount;
        byte _half;
        CRGB *_pixels;
};

#endif
