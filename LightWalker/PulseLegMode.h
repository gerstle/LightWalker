#ifndef PulseLegMode_h
#define PulseLegMode_h

#include "LegMode.h"
#include "FastLED.h"

class PulseLegMode : public LegMode
{
    public:
        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();
        void setSyncData(int syncLength, int syncValue, bool changeColor, bool dimming);


    private:
        CHSV _pulseColor;
        int _pulseLength;
        int _syncLength;
        int _syncValue;
        int _changeColor;
        bool _isDimming;
};

#endif
