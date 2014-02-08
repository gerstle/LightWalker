/*****************************************************************************
 * LWUtils.h
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#ifndef LWUtils_h
#define LWUtils_h

#include "ADXL345_t3.h"
#include "FastLED.h"

class LWUtilsClass
{
    public:
        static void goDark(int pixel_count); 
        static bool debounce(int switchPin, bool lastState);
        static void selectI2CChannels(int channels);
        static void initADXL(ADXL345 *adxl);
        static void printRGB(CRGB color, bool newLine);
        static void printRGB(byte r, byte g, byte b, bool newLine);
        static void setTransitionColor(CRGB* led, double numerator, double denomenator, CHSV fromColor, CHSV toColor);
        static void setTransitionColor(CRGB* led, double numerator, double denomenator, CRGB fromColor, CRGB toColor);
};

extern LWUtilsClass LWUtils;
#endif
