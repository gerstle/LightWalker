#ifndef LWMainConfigs_h
#define LWMainConfigs_h

#include "Enums.h"
#include "colors.h"

class MainConfigs
{
    public:
        MainConfigs();

        int minBrightness;
        int maxBrightness;
        WalkingModeEnum defaultMode;
        bool legsOn;
        bool armsOn;
        RGB minColors[COLOR_COUNT];
        RGB maxColors[COLOR_COUNT];
};

#endif
