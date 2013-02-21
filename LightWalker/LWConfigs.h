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
#define MAX_PIXELS_PER_LEG 50

#define DEBUG 1

enum WalkingModeEnum
{
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

struct PulseConfigs
{
    PulseConfigs() : color(COLOR_WHITE), minBrightness(0), maxBrightness(200), minPulseTime(1000), maxPulseTime(5000) {}

    RGB color;
    int minBrightness;
    int maxBrightness;
    int minPulseTime;
    int maxPulseTime;
};

class LWConfigsClass
{
    public:
        PulseConfigs pulse;
        MainConfigs main;
};

extern LWConfigsClass LWConfigs;
#endif
