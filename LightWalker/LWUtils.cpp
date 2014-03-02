/*****************************************************************************
 * LWUtils.cpp
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "LWUtils.h"
#include "LWConfigs.h"
#ifdef __MK20DX256__
    #include <i2c_t3.h>
#else
    #include <Wire.h>
#endif

LWUtilsClass LWUtils;

// <gerstle> debounce an input switch
bool LWUtilsClass::debounce(int switchPin, bool lastState)
{
    bool currentState = digitalRead(switchPin);

    if (lastState != currentState)
    {
        delay(5);
        currentState = digitalRead(switchPin);
    }

    return currentState;
}

void LWUtilsClass::selectI2CChannels(int channels) 
{
    Wire.beginTransmission(I2C_MULTIPLEXER);
    Wire.write(channels);
    Wire.endTransmission();  
}

void LWUtilsClass::initADXL(ADXL345 *adxl)
{
    adxl->powerOn();
    adxl->setRangeSetting(2);

    //set activity/ inactivity thresholds (0-255)
    adxl->setActivityThreshold(75); //62.5mg per increment
    adxl->setInactivityThreshold(75); //62.5mg per increment
    adxl->setTimeInactivity(10); // how many seconds of no activity is inactive?

    //look of activity movement on this axes - 1 == on; 0 == off 
    adxl->setActivityX(1);
    adxl->setActivityY(1);
    adxl->setActivityZ(1);

    //set values for what is considered freefall (0-255)
    adxl->setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
    adxl->setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
}

void LWUtilsClass::printRGB(CRGB color, bool newLine)
{
    printRGB((byte)color.r, (byte)color.g, (byte)color.b, newLine);
}

void LWUtilsClass::printRGB(byte r, byte g, byte b, bool newLine)
{
    Serial.print(r); Serial.print(", "); Serial.print(g); Serial.print(", "); Serial.print(b);

    if (newLine)
        Serial.println();
}

void LWUtilsClass::setTransitionColor(CRGB* led, double numerator, double denomenator, CHSV fromColor, CHSV toColor)
{
    CRGB from = fromColor;
    CRGB to = toColor;
    setTransitionColor(led, numerator, denomenator, from, to);
}

void LWUtilsClass::setTransitionColor(CRGB* led, double numerator, double denomenator, CRGB fromColor, CRGB toColor)
{
    if (numerator <= 0)
        *led = fromColor;
    else if (numerator >= denomenator)
        *led = toColor;
    else
    {
        CRGB toDelta = toColor;
        toDelta %= (byte)(255 * ((float)numerator/(float)denomenator));

        CRGB fromDelta = fromColor;
        fromDelta %= (byte)(255 * ((float)(denomenator - numerator)/(float)denomenator));

        *led = CRGB::Black;
        *led += fromDelta;
        *led += toDelta;
    }
}
