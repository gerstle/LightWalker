#ifndef LWPulseConfigs_h
#define LWPulseConfigs_h

#include "Enums.h"
#include "colors.h"

class PulseConfigs
{
    public:
        PulseConfigs();

        RGB color;
        int minBrightness;
        int maxBrightness;
        int minPulseTime;
        int maxPulseTime;
        bool randomColor;
        bool syncLegs;
};

#endif
