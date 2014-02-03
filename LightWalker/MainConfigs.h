#ifndef LWMainConfigs_h
#define LWMainConfigs_h

#include "Enums.h"

class MainConfigs
{
    public:
        int maxBrightness = 200;
        WalkingModeEnum defaultMode = sparkle;
        float spectrumMinDB = 20.0; // Audio intensity (in decibels) that maps to low LED brightness.
        float spectrumMaxDB = 40.0; // Audio intensity (in decibels) that maps to high LED brightness.
        int bands = MAX_BANDS; // NEO_PIXEL_COUNT in the spectrum code, I'm using it for frequency bands since I'm not
                               // tying pixels directly to each band
};

#endif
