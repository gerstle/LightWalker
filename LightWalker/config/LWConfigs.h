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

// <gerstle> MICROPHONE
#define AR_PIN 14
#define MIC_PIN 15
#define GAIN_PIN 16

// <gerstle> ACCELEROMETERS
#define I2C_MULTIPLEXER 0x74 // 1110100
#define ADXL_LEFT_ARM 0x1
#define ADXL_LEFT_LEG 0x8
#define ADXL_RIGHT_ARM 0x2
#define ADXL_RIGHT_LEG 0x4

// <gerstle> LEDS
#define LED_CLOCK_PIN 11
#define LED_DATA_PIN 13

#define LEG_PIXEL_COUNT 75
#define LEG_HALF 35
#define ARM_PIXEL_COUNT 100
#define ARM_HALF 50
#define HEAD_PIXEL_COUNT 11
#define LED_COUNT (LEG_PIXEL_COUNT + LEG_PIXEL_COUNT + ARM_PIXEL_COUNT + ARM_PIXEL_COUNT + HEAD_PIXEL_COUNT)

#define LED_STRIPS 5
#define LEDS_PER_STRIP 100

// <gerstle> LIGHTWALKER
#define LEG_COUNT 4

#include "Enums.h"
#include "MainConfigs.h"
#include "PulseConfigs.h"
#include "SparkleConfigs.h"
#include "EqualizerConfigs.h"
#include "GravityConfigs.h"
#include "BubbleConfigs.h"
#include "RainbowConfigs.h"
#include "ZebraConfigs.h"
#include "ChaosConfigs.h"
#include "FlamesConfigs.h"

class LWConfigs
{
    public:
        MainConfigs main;
        PulseConfigs pulse;
        SparkleConfigs sparkle;
        EqualizerConfigs equalizer;
        GravityConfigs gravity;
        BubbleConfigs bubble;
        RainbowConfigs rainbow;
        ZebraConfigs zebra;
        ChaosConfigs chaos;
        FlamesConfigs flames;
};
#endif
