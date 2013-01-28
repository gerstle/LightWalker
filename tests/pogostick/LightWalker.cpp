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

// <gerstle> this is a dummy constructor used by the LightWalker constructor
// to initialize the _legs vector. You must call Leg::init on the 0 index leg
// of the _legs vector during setup
Leg::Leg(int pixel_count) : _pixels(pixel_count, RGB(0x00, 0x00, 0x00))
{
}

// <gerstle> This constructor is used by the initLegs function to add all the legs
// after the first
Leg::Leg(int pin, int pixel_count, int i) : _pixels(pixel_count, RGB(0x00, 0x00, 0x00))
{
//     Serial.println(26);
//     init(pin, pixel_count, i);
//     Serial.println(28);
}

// Leg::Leg(int leg_type, int trigger_pin, int pixel_count)
void Leg::init(int pin, int pixel_count, int i)
{
//     Serial.print("initing..."); Serial.print(pin); Serial.print(" - "); Serial.print(pixel_count); Serial.print(" - "); Serial.println(i);
//     trigger_pin = pin;
//     status = Initialized;
//     _mode = None;
//     legIndex = i;

//     Serial.println(38);
//     // <gerstle> The first leg is initialized by the dummy 
//     // constructor and doesn't have the correct pixel count,
//     // so make sure the _pixels.size() matches the passed in
//     // pixel_count and fix if not
//     Serial.print("pixel size: "); Serial.println(_pixels.size());
//     int j = 0;
//     for (vector<RGB>::iterator it = _pixels.begin(); pixel_count > _pixels.size(); ++it, j++)
//         _pixels.push_back(COLOR_BLACK);
//     Serial.print("pixel size: "); Serial.println(_pixels.size());
//     Serial.print("j: "); Serial.println(j);
//     Serial.println(45);
// 
//     int half = _pixels.size() / 2;
// 
//     if ((_pixels.size() % 2) > 0)
//         lower_foot_border = half - 2;
//     else
//         lower_foot_border = half - 3;
//     upper_foot_border = half + 2;
//     Serial.println(54);
// 
//     off();
//     Serial.println(57);
}

int Leg::pixelCount()
{
    return _pixels.size();
}

void Leg::footdown()
{
    Serial.print("leg "); Serial.print(legIndex); Serial.println(" -> down");
    status = Down;
    _fade_rate = FOOT_DOWN_FADE_STEP;
    flash();
}

void Leg::footup()
{
    Serial.print("leg "); Serial.print(legIndex); Serial.println(" -> up");
    _fade_rate = FOOT_UP_FADE_STEP;
    status = Up;
}

void Leg::sameStatus()
{
//     Serial.print("leg "); Serial.print(legIndex); Serial.println(" -> same");
    unsigned long currentTime;
    switch (_mode)
    {
        case Flash:
            if (currentTime < (_modeChangeTime + FLASH_INTERVAL))
                flash();
            else
                ripple();
            break;

        case Ripple:
            if (currentTime < (_modeChangeTime + RIPPLE_INTERVAL))
                ripple();
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
    int i = 0;
    for (vector<RGB>::iterator it = _pixels.begin(); it != _pixels.end(); ++it, i++)
    {
        if ((i < lower_foot_border) || (i > upper_foot_border))
            setPixel(it, COLOR_BLACK, 0x00);
        else
            setPixel(it, COLOR_WHITE, 0x00);
    }
    _mode = Flash;
}

void Leg::ripple()
{
    byte level;
    for (int i = 0; i < lower_foot_border; i++)
    {
        level = byte(random(0, ((255/lower_foot_border) * (i + 1))));
        _pixels[i] = RGB(level, level, level);
    }

    displayPixels();
    _mode = Ripple;
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
}

void Leg::displayPixels()
{
    for (vector<RGB>::iterator it = _pixels.begin(); it != _pixels.end(); ++it)
        LWUtils.sendColor(*it);
}

void Leg::setPixel(vector<RGB>::iterator it, RGB color, byte dimmer)
{
    it->r = color.r - dimmer;
    it->g = color.g - dimmer;
    it->b = color.b - dimmer;
    LWUtils.sendColor(*it);
}

void Leg::off()
{
    int i = 0;
    for (vector<RGB>::iterator it = _pixels.begin(); it != _pixels.end(); ++it, i++)
        setPixel(it, COLOR_BLACK, 0x00);
    _mode = Off;
}

void Leg::setMode(LightModeEnum mode)
{
    _mode = mode;
    _modeChangeTime = millis();
}

// ----------------------------------------------------------------------------
// LightWalker Class
// ----------------------------------------------------------------------------

LightWalker::LightWalker(int leg_count) : _legs(leg_count, Leg(1))
{
}

void LightWalker::initLegs(vector<int> pins, vector<int> pixel_counts)
{
    TCL.sendEmptyFrame();

    Serial.println(pins.size());
    Serial.println(pixel_counts.size());

    Serial.print(_legs.size()); Serial.println(" starting legs");

    // <gerstle> the constructor only sets 1 leg, so...
    // init the first leg, create the rest
    int i = 0;
    for (vector<int>::iterator it = pins.begin(); it != pins.end(); ++it, i++)
    {
        Serial.print("                  i:"); Serial.println(i);
        if (i == 0)
        {
            Serial.println(213);
            _legs[i].init(*it, pixel_counts[i], i);
            Serial.println(215);
        }
        else
        {
            Serial.println(219);
            _legs.push_back(Leg(*it, pixel_counts[i], i));
            Serial.println(220);
        }
    }

    Serial.print(i); Serial.println(" loops");
    Serial.print(_legs.size()); Serial.println(" legs initialized");
}

void LightWalker::off()
{
    TCL.sendEmptyFrame();
    for (vector<Leg>::iterator it = _legs.begin(); it != _legs.end(); ++it)
        it->off();
}

// <gerstle> check for inputs
void LightWalker:: walk()
{
    // <gerstle> foreach leg, check the sensor
    TCL.sendEmptyFrame();
//     Serial.println("walk");
    for (vector<Leg>::iterator it = _legs.begin(); it != _legs.end(); ++it)
    {
//         Serial.print("    leg "); Serial.println(it->legIndex);
        bool sensorState = digitalRead(it->trigger_pin);

//         Serial.print("        sensor: "); Serial.println(sensorState);
//         Serial.print("        status: "); Serial.println(it->status);

        if ((sensorState == LOW) && (it->status != Down))
            it->footdown();
        else if ((sensorState == HIGH) && (it->status != Up))
            it->footup();
        else
            it->sameStatus();
    }
}
