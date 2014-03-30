#ifndef LWMainConfigs_h
#define LWMainConfigs_h

#include "Enums.h"

class MainConfigs
{
    public:
        MainConfigs() : maxBrightness(200), defaultMode(sparkle), spectrumMinDB(60.0), spectrumMaxDB(90.0), bands(MAX_BANDS) {}

        int maxBrightness;
        WalkingModeEnum defaultMode;
        float spectrumMinDB; // Audio intensity (in decibels) that maps to low LED brightness.
        float spectrumMaxDB; // Audio intensity (in decibels) that maps to high LED brightness.
        int bands; // NEO_PIXEL_COUNT in the spectrum code, I'm using it for frequency bands since I'm not
                   // tying pixels directly to each band
};

#endif
