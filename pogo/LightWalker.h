
/*****************************************************************************
 * LightWalker.h
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#ifndef LightWalker_h
#define LightWalker_h

#include <TCL.h>
#include "LWUtils.h"

#define LEG_COUNT 2
//#define LEG_COUNT 1
#define MAX_PIXELS_PER_LEG 50

// <gerstle> milliseconds for each mode
#define SPARKLE_INTERVAL 500

// <gerstle> different settings for each mode
#define FOOT_DOWN_FADE_STEP 0x5
#define FOOT_UP_FADE_STEP 0x15

#define DEBUG 0

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
    Sparkle,
    Fade,
    Off
};

class Leg
{
    public:
        FootStatusEnum status;
        int trigger_pin;
        int pixel_count;
        String name;
        bool fade_on;
        int foot_down_fade_step;
        int foot_up_fade_step;
        int max_brightness;

        Leg();
        Leg(String n, int trigger_pin, int pixels, int brightness);

        void footdown();
        void footup();
        void sameStatus();
        void off();

    private:
        int lower_foot_border;
        int upper_foot_border;
        LightModeEnum _mode;
        unsigned long _modeChangeTime;

        int _fade_rate;

        void setPixel(int i, RGB color, byte dimmer);
        void displayPixels();
        void flash();
        void sparkle();
        void fade();
        void setMode(LightModeEnum mode);
        RGB _pixels[MAX_PIXELS_PER_LEG];
};

class LightWalker
{
    public:

        LightWalker(int brightness);
        void initLegs();
        void off();
        void walk();

    private:
        Leg _legs[LEG_COUNT];
        unsigned long _laststatus;
        int _max_brightness;
};

#endif
