#ifndef LWBubbleConfigs_h
#define LWBubbleConfigs_h

#include "Enums.h"
#include "FastLED.h"


class BubbleConfigs
{
    public:
        CHSV backgroundColor;
        CHSV bubbleColor;
        int speed = 5;
        int width = 3;
        bool trail = true;
};

#endif
