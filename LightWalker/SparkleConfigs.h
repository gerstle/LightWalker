#ifndef LWSparkleConfigs_h
#define LWSparkleConfigs_h

#include "Enums.h"
#include "Arduino.h"
#include "colors.h"

class SparkleConfigs
{
    public:
        SparkleConfigs();

        RGB footFlashColor;
        RGB sparkleColor;
        int flashLength;
        int sparkleLength;
        byte fadeRate;
};

#endif
