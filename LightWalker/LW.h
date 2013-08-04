
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

#define EQ_EMA_N 200
#define EQ_EMA_PEAK_N 5 

#define LEG_COUNT 4
#define LEFT_LEG_PIXEL_COUNT 50
#define RIGHT_LEG_PIXEL_COUNT 50
#define LEFT_ARM_PIXEL_COUNT 100
#define RIGHT_ARM_PIXEL_COUNT 100

class LW
{
    public:
        LW() : config(), _laststatus(millis()), mode(masterOff), _lightModeChangeTime(millis()), _pulse_length(0), _pulse_isDimming(0) {}
        void initLegs(WalkingModeEnum m);
        void off();
        void walk();
        void equalizer_listen();
        void equalizer_baseline();
        void setMode(WalkingModeEnum m);

        LWConfigs config;
        WalkingModeEnum mode;

//         int eqValues[VALUE_COUNT];
//         int eqValueIndex;
//         double eqValueTotal;
//         int eqValueAvg;

    private:
        Leg _legs[LEG_COUNT];
        RGB _leftLegPixels[LEFT_LEG_PIXEL_COUNT];
        RGB _rightLegPixels[RIGHT_LEG_PIXEL_COUNT];
        RGB _leftArmPixels[LEFT_ARM_PIXEL_COUNT];
        RGB _rightArmPixels[RIGHT_ARM_PIXEL_COUNT];

        unsigned long _laststatus;

        unsigned long _lightModeChangeTime;

        // <gerstle> pulse
        int _pulse_length;
        bool _pulse_isDimming;

        ADXL345 _adxl;
        float eqEMA;
        float eqEMAPeak;
        int eqNminus2;
        int eqNminus1;
};

#endif
