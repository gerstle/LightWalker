/*****************************************************************************
 * configs.h
 *
 * The purpose of this file is to hold configuration settings
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#pragma once

#ifndef LWConfigs_h
#define LWConfigs_h

#define DEBUG 1

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Enums.h"
#include "MainConfigs.h"
#include "PulseConfigs.h"
#include "SparkleConfigs.h"
#include "EqualizerConfigs.h"
#include "GravityConfigs.h"

enum Preferences
{
    // ------------------------------------------------------------------------
    // Main
    // ------------------------------------------------------------------------
    mainMinBrightness,
    mainMaxBrightness,
    mainLegsOn,
    mainArmsOn,
    mainDefaultMode,

    //------------------------------------------------------------------------
    //Mode
    //------------------------------------------------------------------------
    mode,

    //------------------------------------------------------------------------
    // Pulse
    //------------------------------------------------------------------------
    pulseMinRate,
    pulseMaxRate,
    pulseRandomColor,
    pulseSyncLegs,
    pulseColor,

    //------------------------------------------------------------------------
    // Sparkle
    //------------------------------------------------------------------------
    sparkleFootUpFadeRate,
    sparkleFootDownFadeRate,
    sparkleFlashLength,
    sparkleSparkleLength,
    sparkleFootFlashColor,
    sparkleFootSparkleColor,
    sparkleLegSparkleColor,

    //------------------------------------------------------------------------
    // Equalizer
    //------------------------------------------------------------------------
    eqRMSThreshold,
    eqColor,
    eqAllLights,
    eqAllBands,

    //------------------------------------------------------------------------
    // Gravity
    //------------------------------------------------------------------------
    gravityColorOne,
    gravityColorTwo,
    gravityColorThree,
    gravityRotate,
};

class LWConfigs
{
    public:
        LWConfigs() : main(), pulse(), sparkle(), equalizer() {}

        MainConfigs main;
        PulseConfigs pulse;
        SparkleConfigs sparkle;
        EqualizerConfigs equalizer;
        GravityConfigs gravity;
};
#endif
