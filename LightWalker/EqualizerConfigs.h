#ifndef LWEqualizerConfigs_h
#define LWEqualizerConfigs_h

#include "Enums.h"
#include "FastSPI_LED2.h"

enum EqualizerMode
{
    EQStaticColor,
    EQSingleRainbow,
    EQDoubleRainbow
};

class EqualizerConfigs
{
    public:
        EqualizerMode mode = EQDoubleRainbow;
        CHSV color;
        int brightnessPercent = 0;
        bool allBands = false;
        int peak = 0;
        int minValue = 50;
};

#endif
