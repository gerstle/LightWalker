#ifndef LWBubbleConfigs_h
#define LWBubbleConfigs_h

#include "Enums.h"
#include "FastLED.h"

class BubbleConfigs
{
    public:
        BubbleConfigs() : speed(5), width(3), trail(true) {}

        CHSV backgroundColor;
        CHSV bubbleColor;
        int speed;
        int width;
        bool trail;
};

#endif
