#ifndef LWSparkleConfigs_h
#define LWSparkleConfigs_h

#include "Enums.h"
#include "FastLED.h"

class SparkleConfigs
{
    public:
        SparkleConfigs() : footFlashColor(CHSV(255, 0, 255)), sparkleColor(CHSV(255, 0, 255)), flashLength(500), sparkleLength(500), fadeRate(1), minValue(30) {}

        CHSV footFlashColor;
        CHSV sparkleColor;
        int flashLength;
        int sparkleLength;
        byte fadeRate;
        int minValue;
};

#endif
