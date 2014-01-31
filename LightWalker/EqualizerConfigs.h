#ifndef LWEqualizerConfigs_h
#define LWEqualizerConfigs_h

#include "Enums.h"
#include "FastSPI_LED2.h"

class EqualizerConfigs
{
    public:
        CHSV color;
        int brightnessPercent = 0;
        bool allLights = false;
        bool allBands = false;
        int rmsThreshold = 75;
        int peak = 0;
        bool rainbow = false;
        bool fullRainbow = false;
        int minValue = 50;
};

#endif
