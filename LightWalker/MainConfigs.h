#ifndef LWMainConfigs_h
#define LWMainConfigs_h

#include "Enums.h"

class MainConfigs
{
    public:
        MainConfigs();

        int minBrightness;
        int maxBrightness;
        WalkingModeEnum defaultMode;
        bool legsOn;
        bool armsOn;
};

#endif
