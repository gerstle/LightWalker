#ifndef LWMainConfigs_h
#define LWMainConfigs_h

#include "Enums.h"

class MainConfigs
{
    public:
        MainConfigs();

        int maxBrightness;
        WalkingModeEnum defaultMode;
        int legCount;
        int pixelsPerLeg;
};

#endif
