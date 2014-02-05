#ifndef LWZebraConfigs_h
#define LWZebraConfigs_h

#include "FastSPI_LED2.h"

class ZebraConfigs
{
    public:
        CHSV colorOne = CHSV(0, 0, 255);
        CHSV colorTwo = CHSV(0, 0, 0);
        byte stage;
        byte frame;
};

#endif
