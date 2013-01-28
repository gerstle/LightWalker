
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
#include <StandardCplusplus.h>
#include <vector>
#include <iterator>
#include "LWUtils.h"

// <gerstle> milliseconds for each mode
#define FLASH_INTERVAL 500
#define RIPPLE_INTERVAL 500

// <gerstle> different settings for each mode
#define FADE_RATE 250
#define FOOT_DOWN_FADE_STEP 0x20
#define FOOT_UP_FADE_STEP 0x40

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
    Ripple,
    Fade,
    Off
};

using std::vector;

class Leg
{
    public:
        FootStatusEnum status;
        int legIndex;
        int trigger_pin;

        Leg(int pixel_count);
        Leg(int trigger_pin, int pixel_count, int i);

        void init(int trigger_pin, int pixel_count, int i);
        void footdown();
        void footup();
        void sameStatus();
        void off();

        int pixelCount();

    private:
        int lower_foot_border;
        int upper_foot_border;
        LightModeEnum _mode;
        unsigned long _modeChangeTime;

        int _fade_step;
        int _fade_rate;

        void setPixel(vector<RGB>::iterator it, RGB color, byte dimmer);
        void displayPixels();
        void flash();
        void ripple();
        void fade();
        void setMode(LightModeEnum mode);
        vector<RGB> _pixels;
};

class LightWalker
{
    public:
        LightWalker(int leg_count);
        void initLegs(vector<int> pins, vector<int> pixel_counts);
        void off();
        void walk();

    private:
        vector<Leg> _legs;
};

#endif
