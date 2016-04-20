
/*****************************************************************************
 * LW.h
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#ifndef LW_h
#define LW_h

#include "../accelerometer/ADXL345_compatible.h"
#include "FastLED.h"
#include "LWUtils.h"
#include "../config/LWConfigs.h"
#include "Leg.h"

class LW
{
    public:
        LW() : config(), eqLevel(0.0), _mode(masterOff), _pulse_length(0),
				leds_legs(leds),
				leds_left_arm(&(leds[LEG_PIXEL_COUNT * 2])),
				leds_right_arm(&(leds[LEG_PIXEL_COUNT * 2 + ARM_PIXEL_COUNT])),
				leds_horns(&(leds[LEG_PIXEL_COUNT * 2 + ARM_PIXEL_COUNT * 2]))
    			{}

        LWConfigs config;
        CRGB leds[LEG_PIXEL_COUNT + LEG_PIXEL_COUNT + ARM_PIXEL_COUNT + ARM_PIXEL_COUNT + HEAD_PIXEL_COUNT];
        CRGB *leds_legs;
        CRGB *leds_left_arm;
        CRGB *leds_right_arm;
        CRGB *leds_horns;
        float eqLevel;

        void initLegs(WalkingModeEnum m);
        void off();
        void walk();
        void setMode(WalkingModeEnum m);

    private:
        WalkingModeEnum _mode;
        Leg _legs[LEG_COUNT];
        ADXL345 _adxl;

        // <gerstle> pulse
        int _pulse_length;

        void testLeg(byte legIndex, CRGB color);
};

#endif
