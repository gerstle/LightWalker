#ifndef LWSparkleConfigs_h
#define LWSparkleConfigs_h

#include "Enums.h"
#include "FastSPI_LED2.h"

class SparkleConfigs
{
    public:
        CHSV footFlashColor = CHSV(255, 0, 255);
        CHSV sparkleColor = CHSV(255, 0, 255);
        int flashLength = 500;
        int sparkleLength = 500;
        byte fadeRate = 2;
        int minValue = 30;
};

#endif
