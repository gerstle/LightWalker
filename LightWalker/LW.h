
/*****************************************************************************
 * LW.h
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#ifndef LW_h
#define LW_h

#include <ADXL345.h>
#include <TCL.h>

#include "LWUtils.h"
#include "LWConfigs.h"
#include "Leg.h"

#include "MainConfigs.h"
#include "PulseConfigs.h"
#include "SparkleConfigs.h"
#include "EqualizerConfigs.h"

#define VALUE_COUNT 50

class LW
{
    public:
        LW() : config(), _laststatus(millis()), mode(masterOff), _lightModeChangeTime(millis()), _pulse_length(0), _pulse_isDimming(0), valueIndex(0), valueTotal(0), valueAvg(0) {}
        void initLegs(WalkingModeEnum m);
        void off();
        void walk();
        void equalizer_listen();
        void setMode(WalkingModeEnum m);

        LWConfigs config;
        WalkingModeEnum mode;

        int values[VALUE_COUNT];
        int valueIndex;
        double valueTotal;
        int valueAvg;

    private:
        Leg _legs[LEG_COUNT];
        unsigned long _laststatus;

        unsigned long _lightModeChangeTime;

        // <gerstle> pulse
        int _pulse_length;
        bool _pulse_isDimming;

        ADXL345 _adxl;
};

#endif
