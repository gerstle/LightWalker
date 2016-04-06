#ifndef FlamesLegMode_h
#define FlamesLegMode_h

#include "LegMode.h"

class FlamesLegMode : public LegMode
{
    public:
        FlamesLegMode() : _lastStepTime(millis()), _lastFrame(millis()) {}

        virtual void setup(LWConfigs *c, char const *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        unsigned long _currentTime;
        unsigned long _lastStepTime;
        unsigned long _lastFrame;

        unsigned int _cooling = 120;
        unsigned int _sparking = 50;
        // Array of temperature readings at each simulation cell
        byte _heat[ARM_PIXEL_COUNT];

        CRGB heatColor(uint8_t temperature);
        void fire2012();
};

#endif
