
/*****************************************************************************
 * LW.h
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#ifndef LW_h
#define LW_h

#include <TCL.h>
#include "LWUtils.h"
#include "LWConfigs.h"
#include "Leg.h"

class LW
{
    public:

        LW(int x);
        void walk(Leg legs[]);
        void setMode(Leg legs[], WalkingModeEnum mode);

    private:
        unsigned long _laststatus;
        WalkingModeEnum _mode;

        unsigned long _lightModeChangeTime;

        // <gerstle> pulse
        int _pulse_length;
        bool _pulse_isDimming;
};

#endif
