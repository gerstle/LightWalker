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

// void LWUtilsClass::sendColor(RGB color, byte dimmer)
// {
//     TCL.sendColor(color.r - dimmer, color.g - dimmer, color.b - dimmer);
// }
// 
// void LWUtilsClass::sendColor(RGB color)
// {
//     sendColor(color, 0);
// }

void LWUtilsClass::sendColor(byte r, byte g, byte b, byte dimmer)
{
    TCL.sendColor(r - dimmer, g - dimmer, b - dimmer);
}

void LWUtilsClass::sendColor(byte r, byte g, byte b)
{
    sendColor(r, g, b, 0);
}
