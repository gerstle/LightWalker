
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

#include "MainConfigs.h"
#include "PulseConfigs.h"
#include "SparkleConfigs.h"
#include "EqualizerConfigs.h"

class LW
{
    public:
        LW() : config() {}
        void initLegs(WalkingModeEnum m);
        void off();
        void walk();
        byte equalizerListen();
        void equalizerBaseline();
        void setMode(WalkingModeEnum m);

        LWConfigs config;

        CRGB leds[LEG_PIXEL_COUNT + LEG_PIXEL_COUNT + LEFT_ARM_PIXEL_COUNT + RIGHT_ARM_PIXEL_COUNT];

    private:
        WalkingModeEnum _mode = masterOff;
        Leg _legs[LEG_COUNT];

        // <gerstle> pulse
        int _pulse_length = 0;

        // <gerstle> equalizer
        ADXL345 _adxl;
        float eqEMA;
        float eqEMAPeak;
        int eqNminus2;
        int eqNminus1;
        elapsedMillis _lastEQReading;
};

#endif
