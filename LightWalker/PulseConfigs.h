#ifndef LWPulseConfigs_h
#define LWPulseConfigs_h

#include "colors.h"
#include "Enums.h"

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
