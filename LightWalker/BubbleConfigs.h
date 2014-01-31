#ifndef LWBubbleConfigs_h
#define LWBubbleConfigs_h

#include "Enums.h"
#include "FastSPI_LED2.h"


class BubbleConfigs
{
    public:
        CHSV backgroundColor;
        CHSV backgroundColors[3];
        CHSV bubbleColor;
        int speed = 5;
        int width = 3;
        bool trail = true;
};

#endif
