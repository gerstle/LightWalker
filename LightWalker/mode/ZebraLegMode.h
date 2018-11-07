#ifndef ZebraLegMode_h
#define ZebraLegMode_h

#include "LegMode.h"

#define ZEBRA_LIGHTS 7

class ZebraLegMode : public LegMode
{
    public:
        ZebraLegMode() : _lastSlowTimer(millis()), _startStage(0), _frame(0), _frames(4) {}

        virtual void setup(LWConfigs *c, char const *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p, int baseIndex);
        virtual void frame();

    private:
        unsigned long _currentTime;
        unsigned long _lastSlowTimer;

        byte _startStage;
        int _frame;
        int _frames;

        void paintPixel(byte *stage, int index);
};

#endif
