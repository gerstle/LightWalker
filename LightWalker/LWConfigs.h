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

#define LEG_COUNT 2
#define PIXELS_PER_LEG 25

#define DEBUG 1

enum Preferences
{
    // ------------------------------------------------------------------------
    // Main
    // ------------------------------------------------------------------------
	prefMainLegCount,
	prefMainPixelsPerLeg,
	prefMainMaxBrightness,
	prefMainDefaultMode,

	//------------------------------------------------------------------------
	//Mode
	//------------------------------------------------------------------------
	prefMode,

	//------------------------------------------------------------------------
	// Pulse
	//------------------------------------------------------------------------
	prefPulseMinRate,
	prefPulseMaxRate,
	prefPulseRandomColor,
	prefPulseSyncLegs,
	prefPulseColor,

	//------------------------------------------------------------------------
	// Sparkle
	//------------------------------------------------------------------------
	prefSparkleFootUpFadeRate,
	prefSparkleFootDownFadeRate,
	prefSparkleFlashLength,
	prefSparkleSparkleLength,
	prefSparkleFootFlashColor,
	prefSparkleFootSparkleColor,
	prefSparkleLegSparkleColor,

	//------------------------------------------------------------------------
	// Equalizer
	//------------------------------------------------------------------------
	prefEqualizerColor,
	prefEqualizerLevel,
    prefEqualizerRMSThreshold,
    prefEqualizerAllLights,

	//------------------------------------------------------------------------
	// Gravity
	//------------------------------------------------------------------------
    prefGravityXColor,
    prefGravityYColor,
    prefGravityZColor,
    prefGravityRotate,
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
