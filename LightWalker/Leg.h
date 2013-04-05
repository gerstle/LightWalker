
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
        String name;

        Leg();
        void Init(String n, int trigger_pin, WalkingModeEnum mode);
        void off();
        void setWalkingMode(WalkingModeEnum mode);

        // <gerstle> Sparkle
        bool sparkle_fade_on;

        void sparkle_footdown();
        void sparkle_footup();
        void sparkle_sameStatus();

        // <gerstle> pulse
        void pulse_pulse(unsigned long syncCurrentTime, unsigned long syncTime, int syncLength, int syncValue, bool changeColor);

        // <gerstle>
        void equalizer_listen();

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
        RGB _pixels[PIXELS_PER_LEG];
        void _applyMaxBrightness(byte *pixel, float value, int i);

        // <gerstle> Sparkle stuff        
        void _sparkle_flash();
        void _sparkle_sparkle();
        void _sparkle_fade();
        int _sparkle_fade_rate;

        // <gerstle> Pulse stuff
        bool _pulse_isDimming;
        int _pulse_length;
        RGB _pulse_color;
};

#endif
