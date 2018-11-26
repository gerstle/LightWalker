#ifndef PulseLegMode_h
#define PulseLegMode_h

#include "FastLED.h"
#include "../modes/LegMode.h"

class PulseLegMode : public LegMode
{
    public:
        virtual void setup(config::LWConfigs *c, char const *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p, int baseIndex);
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
