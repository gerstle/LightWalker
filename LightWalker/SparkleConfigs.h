#ifndef LWSparkleConfigs_h
#define LWSparkleConfigs_h

#include "colors.h"
#include "Enums.h"
#include "Arduino.h"

class SparkleConfigs
{
    public:
        SparkleConfigs();

        RGB footFlashColor;
        RGB footSparkleColor;
        RGB legSparkleColor;
        int flashLength;
        int sparkleLength;
        byte footDownFadeRate;
        byte footUpFadeRate;
};

#endif
