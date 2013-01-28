/*****************************************************************************
 * LightWalker.cpp
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include  "LightWalker.h"

// ----------------------------------------------------------------------------
// Leg Class
// ----------------------------------------------------------------------------
Leg::Leg()
{
}

Leg::Leg(String n, int pin, int pixels)
{
    Serial.print("initing..."); Serial.print(n); Serial.print(" - "); Serial.print(pin); Serial.print(" - "); Serial.println(pixels);

    name = n;
    trigger_pin = pin;
    pixel_count = pixels;
    status = Initialized;
    _mode = None;

    int half = pixel_count / 2;

    if ((pixel_count % 2) > 0)
        lower_foot_border = half - 2;
    else
        lower_foot_border = half - 3;
    upper_foot_border = half + 2;

    off();
}

void Leg::footdown()
{
    Serial.print(name); Serial.println(" -> down");
    status = Down;
    _fade_rate = FOOT_DOWN_FADE_STEP;
    flash();
}

void Leg::footup()
{
    Serial.print(name); Serial.println(" -> up");
    _fade_rate = FOOT_UP_FADE_STEP;
    status = Up;
    sameStatus();
}

void Leg::sameStatus()
{
//     Serial.print("leg "); Serial.print(name); Serial.println(" -> same");
    unsigned long currentTime = millis();
    switch (_mode)
    {
        case Flash:
//             if (currentTime <= (_modeChangeTime + flash_interval))
//                 flash();
//             else
                sparkle();
            break;

        case Sparkle:
            if (currentTime <= (_modeChangeTime + SPARKLE_INTERVAL))
                sparkle();
            else
                fade();
            break;

        case Fade:
            fade();
            break;

        case Off:
            off();
            break;
    }
}

void Leg::flash()
{
    Serial.println("flashing " + name);
    for (int i = 0; i < pixel_count; i++)
    {
        if ((i < lower_foot_border) || (i > upper_foot_border))
            setPixel(i, COLOR_BLACK, 0x00);
        else
            setPixel(i, COLOR_WHITE, 0x00);
    }
    setMode(Flash);
}

void Leg::sparkle()
{
    Serial.println("sparkling " + name);
    byte level;
    int distance;
    for (int i = 0; i < lower_foot_border; i++)
    {
        distance = lower_foot_border - i;
        level = byte(random(0, ((255/(lower_foot_border * 3)) * (i + 1))));
        _pixels[i] = RGB(level, level, level);
    }

    for (int i = upper_foot_border + 1; i < pixel_count; i++)
    {
        level = byte(random(0, ((255/(lower_foot_border * 3)) * (lower_foot_border - (i - upper_foot_border) + 1))));
        _pixels[i] = RGB(level, level, level);
    }

    displayPixels();
    setMode(Sparkle);
}

// <gerstle> leg w/ 25 pixels is oriented like this (#'s are pixel index):
//
//          0   24
//          1   23
//          2   22
//          3   21
//          4   20
//          5   19
//          6   18
//          7   17
//          8   16
//          9   15
//          10  14
//          11  13
//            12
//
// The bottom contains the 1 (or 2 in the case of even pixel counts) + 2 pixels
// on each side. A fade 
// 
void Leg::fade()
{
    Serial.println("fading " + name);
    off();
}

void Leg::displayPixels()
{
    for (int i = 0; i < pixel_count; i++)
        LWUtils.sendColor(_pixels[i]);
}

void Leg::setPixel(int i, RGB color, byte dimmer)
{
    _pixels[i].r = color.r - dimmer;
    _pixels[i].g = color.g - dimmer;
    _pixels[i].b = color.b - dimmer;
    LWUtils.sendColor(_pixels[i]);
}

void Leg::off()
{
    for (int i = 0; i < pixel_count; i++)
        setPixel(i, COLOR_BLACK, 0x00);
    setMode(Off);
}

void Leg::setMode(LightModeEnum mode)
{
    if (_mode != mode)
        _modeChangeTime = millis();
    _mode = mode;
}

// ----------------------------------------------------------------------------
// LightWalker Class
// ----------------------------------------------------------------------------

LightWalker::LightWalker(int x)
{
}

void LightWalker::initLegs()
{
    TCL.sendEmptyFrame();

    _legs[0] = Leg("left leg", TCL_MOMENTARY1, 25);
    _legs[1] = Leg("right leg", TCL_MOMENTARY2, 25);
}

void LightWalker::off()
{
    TCL.sendEmptyFrame();
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].off();
}

// <gerstle> check for inputs
void LightWalker:: walk()
{
    TCL.sendEmptyFrame();

    unsigned long currentTime = millis();
    bool displayStatus = false;
    if (currentTime > (_laststatus + 2000))
        displayStatus = true;

    // <gerstle> foreach leg, check the sensor
    for (int i = 0; i < LEG_COUNT; i++)
    {
        bool sensorState = digitalRead(_legs[i].trigger_pin);
        
        if (displayStatus)
        {
            Serial.print("    leg "); Serial.println(_legs[i].name);
            Serial.print("        pin: "); Serial.println(_legs[i].trigger_pin);
            Serial.print("        pin state: "); Serial.println(sensorState);
            Serial.print("        status: "); Serial.println(_legs[i].status);
            _laststatus = millis();
        }

        if ((sensorState == LOW) && (_legs[i].status != Down))
            _legs[i].footdown();
        else if ((sensorState == HIGH) && (_legs[i].status != Up))
            _legs[i].footup();
        else
            _legs[i].sameStatus();
    }
}
