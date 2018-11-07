#ifndef BubbleLegMode_h
#define BubbleLegMode_h

#include "LegMode.h"

class BubbleLegMode : public LegMode
{
    public:
        BubbleLegMode() : _timer(millis()) {}

        virtual void setup(LWConfigs *c, char const *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p, int baseIndex);
        virtual void frame();

    private:
        unsigned long _timer;
        int _leadingBubbleBottom;
        int _trailingBubbleBottom;
        bool _bubbleOn;
};

#endif
