#ifndef ZebraLegMode_h
#define ZebraLegMode_h

#include "LegMode.h"

#define ZEBRA_LIGHTS 6

class ZebraLegMode : public LegMode
{
    public:
        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        elapsedMillis _lastStepTimer = 0;
        elapsedMillis _lastSlowTimer = 0;

        byte _startStage = 0;
        int _frame = 0;
        int _frames = 4;

        void paintPixel(byte *stage, int index);
};

#endif
