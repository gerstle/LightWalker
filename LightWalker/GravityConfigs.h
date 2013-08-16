#ifndef LWGravityConfigs_h
#define LWGravityConfigs_h

#include "Enums.h"
#include "Arduino.h"
#include "colors.h"


class GravityConfigs
{
    public:
        GravityConfigs();

        RGB colorOne;
        RGB colorTwo;
        RGB colorThree;
        bool rotate;
};

#endif
