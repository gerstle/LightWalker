
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

#define ADXL_VALUE_COUNT 100

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
        void init(LWConfigs *c, char *n, int i2c_channel, WalkingModeEnum mode, ADXL345 *adxl, byte count, byte half, RGB *p);
        void setPixelCount(byte count, byte half);
        void off();
        void setWalkingMode(WalkingModeEnum mode, ADXL345 *adxl);
        bool detectStep(ADXL345 *adxl);
        void upToMin(byte *r, byte *g, byte *b, RGB defaultColor);

        // <gerstle> Sparkle
        void sparkle_footdown();
        void sparkle_sameStatus();

        // <gerstle> pulse
        void pulse_pulse(unsigned long syncTime, int syncLength, int syncValue, bool changeColor);

        // <gerstle> eq
        void equalizer_listen(float level_percent, byte r, byte g, byte b);

        // <gerstle> gravity
        void gravity2Lights(ADXL345 *adxl);

        // <cgerstle> bubble
        void bubble_bubble();
        void bubble_step();

    private:
        LWConfigs *config;

        // <gerstle> Generic stuff
        int _lower_foot_border;
        int _upper_foot_border;
        LightModeEnum _lightMode;
        unsigned long _lightModeChangeTime;
        WalkingModeEnum _walkingMode;
        //RGB _pixels[PIXELS_PER_LEG];
        RGB *_pixels;
        byte _pixelCount;
        byte _half;

        void _setPixel(int i, RGB color, byte dimmer);
        void _setLightMode(LightModeEnum mode);
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

        // <cgerstle> equalizer
        int _rainbowColorCount;
        int _pixelsPerColor;

        // <gerstle> bubble
        int _leadingBubbleBottom;
        int _trailingBubbleBottom;
        bool _bubbleOn;
};

#endif
