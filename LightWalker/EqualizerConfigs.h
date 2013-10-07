#ifndef LWEqualizerConfigs_h
#define LWEqualizerConfigs_h

#include "Enums.h"
#include "colors.h"

class EqualizerConfigs
{
    public:
        EqualizerConfigs();

        RGB color;
        RGB minColor;
        RGB maxColor;
        int brightnessPercent;
        bool allLights;
        bool allBands;
        int RMSThreshold;
        int peak;
        bool rainbow;
        bool fullRainbow;
};

#endif
