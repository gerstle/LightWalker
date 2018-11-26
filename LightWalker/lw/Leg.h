
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
#include "../accelerometer/ADXL345_compatible.h"
#include "../config/LWConfigs.h"
#include "FastLED.h"

// <cgerstle> leg modes are what control the lights for each mode
// each leg has one because a lot of the settings/state is leg
// specific
#include "../modes/BubbleLegMode.h"
#include "../modes/FlamesLegMode.h"
#include "../modes/GravityLegMode.h"
#include "../modes/LegMode.h"
#include "../modes/PulseLegMode.h"
#include "../modes/RainbowLegMode.h"
#include "../modes/SparkleLegMode.h"
#include "../modes/ZebraLegMode.h"

#define ADXL_VALUE_COUNT 100


class Leg
{
    public:
        Leg();
        // <gerstle> Generic
        char const *name;
        CRGB *pixels;
        byte pixelCount;
        int baseIndex;

//        PulseLegMode pulseLegMode;
//        SparkleLegMode sparkleLegMode;
//        EqualizerLegMode equalizerLegMode;
//        GravityLegMode gravityLegMode;
//        BubbleLegMode bubbleLegMode;
//        RainbowLegMode rainbowLegMode;
//        ZebraLegMode zebraLegMode;
//        ChaosLegMode chaosLegMode;
//        FlamesLegMode flamesLegMode;

        void init(config::LWConfigs *c, char const *n, int i2c_channel, WalkingModeEnum mode, ADXL345 *adxl, byte count, byte half, CRGB *p, int octoIndex);
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
        config::LWConfigs *_config;
        byte _channel;
        ADXL345 *_adxl;
        LegMode *_leg_mode;
        byte _half;

        // <cgerstle> steps
        unsigned long lastStepTimer;
        float xEMA;
        float yEMA;
        float zEMA;

        int xNMinus1;
        int xNMinus2;

        int yNMinus1;
        int yNMinus2;

        int zNMinus1;
        int zNMinus2;

        int xStepDuration; // <cgerstle> a step lasts at least this long... ie, two steps can't occur within this time period
        int yStepDuration;
        int zStepDuration;
        int xAvgDiffThreshold;
        int yAvgDiffThreshold;
        int zAvgDiffThreshold;

        int xSignificantlyLowerThanAverageThreshold;
        bool readyForStep;
};

#endif
