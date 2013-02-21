
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
        void initLegs(WalkingModeEnum mode);
        void off();
        void walk();
        void setMode(WalkingModeEnum mode);

    private:
        Leg _legs[LEG_COUNT];
        unsigned long _laststatus;
        WalkingModeEnum _mode;
};

#endif
