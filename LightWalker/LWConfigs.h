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
#define AUDIO_PIN 9         // read right channel from analog input 9
#define AUDIO_STROBE_PIN 14  // strobe is attached to digital pin 14
#define AUDIO_RESET_PIN 15   // reset is attached to digital pin 15
#define EQ_EMA_N 200
#define EQ_EMA_PEAK_N 20

// <gerstle> ACCELEROMETERS
#define TCA9548AADDR 0x74 // 1110100
#define ADXL_ONE 0x1
#define ADXL_TWO 0x2
#define ADXL_THREE 0x4
#define ADXL_FOUR 0x8

// <gerstle> LEDS
#define LEG_PIXEL_COUNT 75
#define LEG_HALF 35
#define LEFT_ARM_PIXEL_COUNT 100
#define LEFT_ARM_HALF 50
#define RIGHT_ARM_PIXEL_COUNT 102 // <gerstle> right arm has the hat attached with 2 pixels
#define RIGHT_ARM_HALF 50
#define LED_COUNT (LEG_PIXEL_COUNT + LEG_PIXEL_COUNT + LEFT_ARM_PIXEL_COUNT + RIGHT_ARM_PIXEL_COUNT)

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
};
#endif
