#ifndef LWPulseConfigs_h
#define LWPulseConfigs_h

#include "Enums.h"
#include "FastLED.h"

enum PulseMode
{
    staticColor,
    cycleColor,
    randomColor
};

class PulseConfigs
{
    public:
        CHSV color = CHSV(75, 255, 255);
        int minPulseTime = 1000;
        int maxPulseTime = 5000;
        bool syncLegs = false;
        PulseMode mode = cycleColor;
        elapsedMillis syncLegsTimer;
};

#endif
