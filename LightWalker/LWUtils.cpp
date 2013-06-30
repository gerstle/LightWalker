/*****************************************************************************
 * LWUtils.cpp
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "LWUtils.h"

LWUtilsClass LWUtils;

// <gerstle> turn all the lights off
void LWUtilsClass::goDark(int pixel_count)
{
    TCL.sendEmptyFrame();
    for (int i = 0; i < pixel_count; i++)
        TCL.sendColor(0x00, 0x00, 0x00);
    TCL.sendEmptyFrame();
}

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

void LWUtilsClass::sendColor(RGB color, byte dimmer)
{
    TCL.sendColor(color.r - dimmer, color.g - dimmer, color.b - dimmer);
}

void LWUtilsClass::sendColor(RGB color)
{
    sendColor(color, 0);
}

void LWUtilsClass::selectI2CChannels(int channels) 
{
    Wire.beginTransmission(TCA9548AADDR);
    Wire.write(channels);
    Wire.endTransmission();  
}

void LWUtilsClass::initADXL(ADXL345 *adxl)
{
    adxl->powerOn();

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

void LWUtilsClass::printRGB(RGB color, bool newLine)
{
    printRGB(color.r, color.g, color.b, newLine);
}

void LWUtilsClass::printRGB(byte r, byte g, byte b, bool newLine)
{
    Serial.print(r); Serial.print(", "); Serial.print(g); Serial.print(", "); Serial.print(b);

    if (newLine)
        Serial.println();
}
