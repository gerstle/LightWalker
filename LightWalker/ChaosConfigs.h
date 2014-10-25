#ifndef LWChaosConfigs_h
#define LWChaosConfigs_h

#include "FastLED.h"

class ChaosConfigs
{
    public:
        ChaosConfigs() : color(CHSV(18, 255, 255)), minValue(100), stepLength(1250), delay(8) {}

        byte minValue;
        int stepLength;
        byte delay;
        CHSV color;
        byte swing;
};

#endif
