/*****************************************************************************
 * configs.h
 *
 * The purpose of this file is to hold configuration settings
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#ifndef LWConfigs_h
#define LWConfigs_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "colors.h"

#define LEG_COUNT 2
#define PIXELS_PER_LEG 25

#define DEBUG 1

enum WalkingModeEnum
{
    MasterOff,
    Main,
    Gravity,
    Equalizer,
    Sparkle,
    Pulse
};

struct MainConfigs
{
    MainConfigs() : maxBrightness(200), defaultMode(Pulse) {}

    int maxBrightness;
    WalkingModeEnum defaultMode;
};

struct SparkleConfigs
{
    SparkleConfigs() : footFlashColor(COLORS[WHITE]), footSparkleColor(COLORS[WHITE]), legSparkleColor(COLORS[WHITE]), footDownFadeRate(0x4), footUpFadeRate(0x8), flashLength(500), sparkleLength(500) {}

    RGB footFlashColor;
    RGB footSparkleColor;
    RGB legSparkleColor;
    int flashLength;
    int sparkleLength;
    byte footDownFadeRate;
    byte footUpFadeRate;
};

struct PulseConfigs
{
    PulseConfigs() : color(COLORS[WHITE]), minBrightness(0), maxBrightness(200), minPulseTime(1000), maxPulseTime(5000), randomColor(false), syncLegs(false) {}

    RGB color;
    int minBrightness;
    int maxBrightness;
    int minPulseTime;
    int maxPulseTime;
    bool randomColor;
    bool syncLegs;
};

struct EqualizerConfigs
{
    EqualizerConfigs() : color(COLORS[WHITE]) {}
    RGB color;
    int brightnessPercent;
};

class LWConfigsClass
{
    public:
        MainConfigs main;

        PulseConfigs pulse;
        SparkleConfigs sparkle;
        EqualizerConfigs equalizer;
};

extern LWConfigsClass LWConfigs;
#endif
