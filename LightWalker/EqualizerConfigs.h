#ifndef LWEqualizerConfigs_h
#define LWEqualizerConfigs_h

#include "Enums.h"
#include "FastLED.h"

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
