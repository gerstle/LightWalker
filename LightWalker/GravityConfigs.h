#ifndef LWGravityConfigs_h
#define LWGravityConfigs_h

#include "colors.h"
#include "Enums.h"
#include "Arduino.h"

class GravityConfigs
{
    public:
        GravityConfigs();

        RGB xColor;
        RGB yColor;
        RGB zColor;
        bool rotate;
};

#endif
