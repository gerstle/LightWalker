#ifndef LegMode_h
#define LegMode_h

#include "LWConfigs.h"
#include "ADXL345_t3.h"

class LegMode
{
    public:
        bool stepDetected = false;

        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p) = 0;
        virtual void frame() = 0;

    protected:
        bool _setup_complete = false;

        LWConfigs *_config;
        char *_legName;
        byte _channel;
        ADXL345 *_adxl;
        byte _pixelCount;
        byte _half;
        CRGB *_pixels;
};

#endif
