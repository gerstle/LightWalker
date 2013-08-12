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
        RGB sparkleColor;
        int flashLength;
        int sparkleLength;
        byte fadeRate;
};

#endif
