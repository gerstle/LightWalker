
/*****************************************************************************
 * Leg.h
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#ifndef Leg_h
#define Leg_h

#include <TCL.h>
#include <ADXL345.h>
#include "LWUtils.h"
#include "LWConfigs.h"

#define ADXL_VALUE_COUNT 60

enum FootStatusEnum
{
    Initialized,
    Down,
    Up 
};

enum LightModeEnum
{
    None,
    Flash,
    SparkleSparkle,
    Fade,
    Off
};

class Leg
{
    public:
        // <gerstle> Generic
        FootStatusEnum status;
        int channel;
        char name[10];
        bool step;
        unsigned long currentTime;

        Leg();
        void Init(LWConfigs *c, char n[], int i2c_channel, WalkingModeEnum mode, ADXL345 *adxl, byte half);
        void off();
        void setWalkingMode(WalkingModeEnum mode, ADXL345 *adxl);
        void detectStep(ADXL345 *adxl);
        void upToMin(byte *r, byte *g, byte *b, RGB defaultColor);

        // <gerstle> Sparkle
        bool sparkle_fade_on;

        void sparkle_footdown();
        void sparkle_footup();
        void sparkle_sameStatus();

        // <gerstle> pulse
        void pulse_pulse(unsigned long syncTime, int syncLength, int syncValue, bool changeColor);

        // <gerstle> eq
        void equalizer_listen(float level_percent, byte r, byte g, byte b);

        // <gerstle> gravity
        void gravity2Lights(ADXL345 *adxl);

    private:
        LWConfigs *config;

        // <gerstle> Generic stuff
        int lower_foot_border;
        int upper_foot_border;
        LightModeEnum _lightMode;
        unsigned long _lightModeChangeTime;
        WalkingModeEnum _walkingMode;
        byte _half;

        void _setPixel(int i, RGB color, byte dimmer);
        void _displayPixels();
        void _setLightMode(LightModeEnum mode);
        RGB _pixels[PIXELS_PER_LEG];
        void _applyMaxBrightness(byte *pixel, float value, int i);

        // <gerstle> Sparkle stuff        
        void _sparkle_flash();
        void _sparkle_shimmer();
        void _sparkle_sparkle();
        void _sparkle_fade();
        int _sparkle_fade_rate;

        // <gerstle> Pulse stuff
        bool _pulse_isDimming;
        int _pulse_length;
        RGB _pulse_color;

        // <gerstle> gravity stuff
        byte _indexOne;
        byte _indexTwo;
        byte _indexThree;
        unsigned long _lastXSwitch;
        unsigned long _lastYSwitch;
        bool _canXSwitch;
        bool _canYSwitch;

        // <cgerstle> steps
        unsigned long lastSharpPeakTime;
        int valueIndex;
        int xValues[ADXL_VALUE_COUNT];
        int yValues[ADXL_VALUE_COUNT];
        int zValues[ADXL_VALUE_COUNT];

        int xTotal;
        int xAverage;
        int xAverageOld;

        int yTotal;
        int yAverage;
        int yAverageOld;

        int zTotal;
        int zAverage;
        int zAverageOld;

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
