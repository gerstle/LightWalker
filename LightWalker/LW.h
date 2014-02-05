
/*****************************************************************************
 * LW.h
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#ifndef LW_h
#define LW_h

#include "ADXL345_t3.h"
#include "FastSPI_LED2.h"
#include "LWUtils.h"
#include "LWConfigs.h"
#include "Leg.h"

class LW
{
    public:
        LWConfigs config;
        CRGB leds[LEG_PIXEL_COUNT + LEG_PIXEL_COUNT + LEFT_ARM_PIXEL_COUNT + RIGHT_ARM_PIXEL_COUNT];
        float eqLevel = 0.0;

        LW() : config() {}
        void initLegs(WalkingModeEnum m);
        void off();
        void walk();
        void setMode(WalkingModeEnum m);

    private:
        WalkingModeEnum _mode = masterOff;
        Leg _legs[LEG_COUNT];
        ADXL345 _adxl;

        // <gerstle> pulse
        int _pulse_length = 0;

        void testLeg(byte legIndex, CRGB color);
};

#endif
