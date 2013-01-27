
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

using std::vector;

class Leg
{
    public:
        Leg(int x);
        void Init(int trigger_pin, int pixel_count);
        int triggerPin();
        bool lastTriggerStatus();
        void setLastTriggerStatus(bool status);
        void flash();
        void setPixel(vector<RGB>::iterator it, RGB color, byte dimmer);
        void repeatFrame();
        int pixelCount();
        void setSolidColor(RGB color, byte dimmer);
    private:
        int _trigger_pin;
        bool _last_trigger_status;
        vector<RGB> _current_pixels;
};

class LightWalker
{
    public:
        LightWalker(int leg_count);
        void InitLegs(vector<int> pins, vector<int> pixel_counts);
        void lowGlow();
        void off();
        void walk();
    private:
        vector<Leg> _legs;
};

#endif
