#ifndef LWGravityConfigs_h
#define LWGravityConfigs_h

#include "Enums.h"
#include "FastLED.h"

class GravityConfigs
{
    public:
        GravityConfigs() : rotate(false), minValue(40) {}

        bool rotate;
        byte minValue;
};

#endif
