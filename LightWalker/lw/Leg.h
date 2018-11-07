
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
#include "../mode/LegMode.h"
#include "../mode/PulseLegMode.h"
#include "../mode/SparkleLegMode.h"
#include "../mode/EqualizerLegMode.h"
#include "../mode/GravityLegMode.h"
#include "../mode/BubbleLegMode.h"
#include "../mode/RainbowLegMode.h"
#include "../mode/ZebraLegMode.h"
#include "../mode/ChaosLegMode.h"
#include "../mode/FlamesLegMode.h"

#define ADXL_VALUE_COUNT 100


class Leg
{
    public:
        Leg() : name(NULL),
				pixels(NULL),
				pixelCount(0),
				baseIndex(0),
                _config(NULL),
                _adxl(NULL),
                _leg_mode(NULL),
				_half(0),
                lastStepTimer(millis()),
                xEMA(0),
                yEMA(0),
                zEMA(0),
                xNMinus1(0),
                xNMinus2(0),
                yNMinus1(0),
                yNMinus2(0),
                zNMinus1(0),
                zNMinus2(0),
                xStepDuration(150),
                yStepDuration(350),
                zStepDuration(350),
                xAvgDiffThreshold(170),
                yAvgDiffThreshold(150),
                zAvgDiffThreshold(150),
                xSignificantlyLowerThanAverageThreshold(45),
                readyForStep(true) {}

        // <gerstle> Generic
        char const *name;
        CRGB *pixels;
        byte pixelCount;
        int baseIndex;

        PulseLegMode pulseLegMode;
        SparkleLegMode sparkleLegMode;
        EqualizerLegMode equalizerLegMode;
        GravityLegMode gravityLegMode;
        BubbleLegMode bubbleLegMode;
        RainbowLegMode rainbowLegMode;
        ZebraLegMode zebraLegMode;
        ChaosLegMode chaosLegMode;
        FlamesLegMode flamesLegMode;

        void init(LWConfigs *c, char const *n, int i2c_channel, WalkingModeEnum mode, ADXL345 *adxl, byte count, byte half, CRGB *p, int octoIndex);
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
        LWConfigs *_config;
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
