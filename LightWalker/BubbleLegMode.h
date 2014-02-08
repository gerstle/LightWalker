#ifndef BubbleLegMode_h
#define BubbleLegMode_h

#include "LegMode.h"

class BubbleLegMode : public LegMode
{
    public:
        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        elapsedMillis _timer = 0;
        int _leadingBubbleBottom;
        int _trailingBubbleBottom;
        bool _bubbleOn;
};

#endif
