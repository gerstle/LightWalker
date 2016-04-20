#ifndef LWMainConfigs_h
#define LWMainConfigs_h

#include "Enums.h"

class MainConfigs
{
    public:
        MainConfigs() : maxBrightness(200), defaultMode(sparkle) {}

        int maxBrightness;
        WalkingModeEnum defaultMode;
};

#endif
