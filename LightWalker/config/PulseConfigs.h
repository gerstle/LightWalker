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
        PulseConfigs() : color(CHSV(75, 255, 255)), minPulseTime(1000), maxPulseTime(5000), syncLegs(false), mode(cycleColor) {}

        CHSV color;
        int minPulseTime;
        int maxPulseTime;
        bool syncLegs;
        PulseMode mode;
        unsigned long syncLegsTimer;
};

#endif
