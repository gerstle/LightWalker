
/*****************************************************************************
 * Leg.h
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#ifndef Leg_h
#define Leg_h

#include "LWUtils.h"
#include "ADXL345_t3.h"
#include "LWConfigs.h"
#include "FastSPI_LED2.h"

// <cgerstle> leg modes are what control the lights for each mode
// each leg has one because a lot of the settings/state is leg
// specific
#include "LegMode.h"
#include "RainbowLegMode.h"
#include "PulseLegMode.h"
#include "SparkleLegMode.h"
#include "EqualizerLegMode.h"
#include "GravityLegMode.h"
#include "BubbleLegMode.h"

#define ADXL_VALUE_COUNT 100


class Leg
{
    public:
        // <gerstle> Generic
        char *name;

        RainbowLegMode rainbowLegMode;
        PulseLegMode pulseLegMode;
        SparkleLegMode sparkleLegMode;
        EqualizerLegMode equalizerLegMode;
        GravityLegMode gravityLegMode;
        BubbleLegMode bubbleLegMode;

        void init(LWConfigs *c, char *n, int i2c_channel, WalkingModeEnum mode, ADXL345 *adxl, byte count, byte half, CRGB *p);
        void off();
        void setWalkingMode(WalkingModeEnum mode);
        bool detectStep();
        void frame();

        // <cgerstle> bubble
        void bubble_bubble();
        void bubble_step();

    private:
        // <gerstle> Generic stuff
        WalkingModeEnum _walkingMode;
        LWConfigs *_config = NULL;
        byte _channel;
        ADXL345 *_adxl = NULL;
        LegMode *_leg_mode = NULL;
        CRGB *_pixels = NULL;
        byte _pixelCount;
        byte _half;

        // <cgerstle> steps
        elapsedMillis lastSharpPeakTime;
        float xEMA;
        float yEMA;
        float zEMA;

        int xNMinus1;
        int xNMinus2;

        int yNMinus1;
        int yNMinus2;

        int zNMinus1;
        int zNMinus2;

        int xStepDuration = 350; // <cgerstle> a step lasts at least this long... ie, two steps can't occur within this time period
        int yStepDuration = 750;
        int zStepDuration = 750;
        int xAvgDiffThreshold = 160;
        int yAvgDiffThreshold = 80;
        int zAvgDiffThreshold = 80;

        int xSignificantlyLowerThanAverageThreshold = 45;
        bool readyForStep = false;
};

#endif
