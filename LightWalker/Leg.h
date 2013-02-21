
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
#include "LWUtils.h"
#include "LWConfigs.h"

// <gerstle> milliseconds for each mode
#define SPARKLE_INTERVAL 500

// <gerstle> sparkle settings
#define SPARKLE_FOOT_DOWN_FADE_STEP 0x5
#define SPARKLE_FOOT_UP_FADE_STEP 0x15

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
        int trigger_pin;
        int pixel_count;
        String name;

        Leg();
        Leg(String n, int trigger_pin, int pixels, int x, WalkingModeEnum mode);
        void off();
        void setWalkingMode(WalkingModeEnum mode);

        // <gerstle> Sparkle
        bool sparkle_fade_on;
        int sparkle_foot_down_fade_step;
        int sparkle_foot_up_fade_step;

        void sparkle_footdown();
        void sparkle_footup();
        void sparkle_sameStatus();

        // <gerstle> pulse
        void pulse_pulse();

    private:
        // <gerstle> Generic stuff
        int lower_foot_border;
        int upper_foot_border;
        LightModeEnum _lightMode;
        unsigned long _lightModeChangeTime;
        WalkingModeEnum _walkingMode;

        void _setPixel(int i, RGB color, byte dimmer);
        void _displayPixels();
        void _setLightMode(LightModeEnum mode);
        RGB _pixels[MAX_PIXELS_PER_LEG];
        void _applyMaxBrightness(byte *pixel, float value, int i);

        // <gerstle> Sparkle stuff        
        void _sparkle_flash();
        void _sparkle_sparkle();
        void _sparkle_fade();
        int _sparkle_fade_rate;

        // <gerstle> Pulse stuff
        bool _pulse_isDimming;
        int _pulse_length;
};

#endif
