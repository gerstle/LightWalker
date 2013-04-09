#ifndef LWEqualizerConfigs_h
#define LWEqualizerConfigs_h

#include "colors.h"
#include "Enums.h"

class EqualizerConfigs
{
    public:
        EqualizerConfigs();

        RGB color;
        int brightnessPercent;
        bool allLights;
        int RMSThreshold;
};

#endif
