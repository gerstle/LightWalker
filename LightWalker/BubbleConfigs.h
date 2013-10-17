#ifndef LWBubbleConfigs_h
#define LWBubbleConfigs_h

#include "Enums.h"
#include "Arduino.h"
#include "colors.h"


class BubbleConfigs
{
    public:
        BubbleConfigs();
        RGB backgroundColor;
        RGB backgroundColors[3];
        RGB bubbleColor;
        int speed;
        int width;
        bool trail;
};

#endif
