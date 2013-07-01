#ifndef LWEqualizerConfigs_h
#define LWEqualizerConfigs_h

#include "colors.h"
#include "Enums.h"

class EqualizerConfigs
{
    public:
        EqualizerConfigs();

        RGB color;
        RGB minColor;
        int brightnessPercent;
        bool allLights;
        bool allBands;
        int RMSThreshold;
        int peak;
};

#endif
