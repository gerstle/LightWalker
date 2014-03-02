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
        EqualizerConfigs() : mode(EQDoubleRainbow), brightnessPercent(0), allBands(false), peak(0), minValue(50) {}

        EqualizerMode mode;
        CHSV color;
        int brightnessPercent;
        bool allBands;
        int peak;
        int minValue;
};

#endif
