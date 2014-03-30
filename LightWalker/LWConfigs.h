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
#ifdef __MK20DX256__
    #define AUDIO_PIN 14
    #define ANALOG_READ_RESOLUTION 10   // Bits of resolution for the ADC.
    #define ANALOG_READ_AVERAGING 16    // Number of samples to average with each ADC reading.
    #define SAMPLE_RATE_HZ 8000         // Sample rate of the audio in hertz.
    #define FFT_SIZE 256                // Size of the FFT.  Realistically can only be at most 256 
                                        // without running out of memory for buffers and other state.
#else
    #define AUDIO_PIN 1
    #define AUDIO_STROBE_PIN 4  // strobe is attached to digital pin 4
    #define AUDIO_RESET_PIN 5   // reset is attached to digital pin 5
#endif
#define MAX_BANDS 6
#define EQ_EMA_N 200
#define EQ_EMA_PEAK_N 20

// <gerstle> ACCELEROMETERS
#define I2C_MULTIPLEXER 0x74 // 1110100
#define ADXL_ONE 0x1
#define ADXL_TWO 0x2
#define ADXL_THREE 0x4
#define ADXL_FOUR 0x8

// <gerstle> LEDS
#ifdef __MK20DX256__
    #define LED_CLOCK_PIN 11
    #define LED_DATA_PIN 13
#else
    #define LED_CLOCK_PIN 51
    #define LED_DATA_PIN 52
#endif
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
#include "ZebraConfigs.h"
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
        FlamesConfigs flames;
};
#endif
