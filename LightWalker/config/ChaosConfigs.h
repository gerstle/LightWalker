#ifndef LWChaosConfigs_h
#define LWChaosConfigs_h

#include "FastLED.h"

class ChaosConfigs
{
    public:
        ChaosConfigs() : color(CHSV(18, 255, 255)), minValue(100), stepLength(1250), speed(8), swing(40), sparse(0) {}

        CHSV color;
        byte minValue;
        int stepLength;
        byte speed;
        byte swing;
        boolean sparse;
};

#endif
