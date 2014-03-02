#ifndef LWZebraConfigs_h
#define LWZebraConfigs_h

#include "FastLED.h"

class ZebraConfigs
{
    public:
        ZebraConfigs() : colorOne(CHSV(0, 0, 255)), colorTwo(CHSV(0, 0, 0)) {}

        CHSV colorOne;
        CHSV colorTwo;
        byte stage;
        byte frame;
};

#endif
