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

Leg::Leg(int pixel_count) : _current_pixels(pixel_count, RGB(0x00, 0x00, 0x00))
{
}

// Leg::Leg(int leg_type, int trigger_pin, int pixel_count)
void Leg::Init(int trigger_pin, int pixel_count)
{
    // <gerstle> initialization creates a single pixel, so...
    // set first pixel to black and add pixel_count - 1 more
    _current_pixels[0].r = COLOR_BLACK.r;
    _current_pixels[0].b = COLOR_BLACK.b;
    _current_pixels[0].g = COLOR_BLACK.g;

    for (int i = 0; i < (pixel_count - 1); i++)
        _current_pixels.push_back(COLOR_BLACK);

    _trigger_pin = trigger_pin;
    _last_trigger_status = HIGH;
}

int Leg::pixelCount()
{
    return _current_pixels.size();
}

int Leg::triggerPin()
{
    return _trigger_pin;
}

bool Leg::lastTriggerStatus()
{
    return _last_trigger_status;
}

void Leg::setLastTriggerStatus(bool status)
{
    _last_trigger_status = status;
}

void Leg::flash()
{
    Serial.println("flashing");
    int half = _current_pixels.size() / 2;
    int i = 0;
    for (vector<RGB>::iterator it = _current_pixels.begin(); it != _current_pixels.end(); ++it, i++)
    {
        if ((i < (half - 3)) || (i > (half + 2)))
            setPixel(it, COLOR_BLACK, 0x00);
        else
            setPixel(it, COLOR_WHITE, 0x00);
    }
}

void Leg::setSolidColor(RGB color, byte dimmer)
{
    int i = 0;
    for (vector<RGB>::iterator it = _current_pixels.begin(); it != _current_pixels.end(); ++it, i++)
        setPixel(it, color, dimmer);
}

void Leg::setPixel(vector<RGB>::iterator it, RGB color, byte dimmer)
{
    it->r = color.r - dimmer;
    it->g = color.g - dimmer;
    it->b = color.b - dimmer;
    LWUtils.sendColor(*it);
}

void Leg::repeatFrame()
{
//     Serial.println("repeating");
//     int count = pixelCount();
//     for (int j = 0; j < count; j++)
//         LWUtils.sendColor(_current_pixels[j]);
}

// ----------------------------------------------------------------------------
// LightWalker Class
// ----------------------------------------------------------------------------

LightWalker::LightWalker(int leg_count) : _legs(leg_count, Leg(1))
{
}

void LightWalker::InitLegs(vector<int> pins, vector<int> pixel_counts)
{
    int i = 0;
    for (vector<Leg>::iterator it = _legs.begin(); it != _legs.end(); ++it, i++)
        it->Init(pins[i], pixel_counts[i]);
}

void LightWalker::off()
{
    int leg_count = _legs.size();
    TCL.sendEmptyFrame();
    int i = 0;
    for (vector<Leg>::iterator it = _legs.begin(); it != _legs.end(); ++it, i++)
        it->setSolidColor(COLOR_BLACK, 0x00);
}

void LightWalker::lowGlow()
{
    int leg_count = _legs.size();
    TCL.sendEmptyFrame();
    int i = 0;
    for (vector<Leg>::iterator it = _legs.begin(); it != _legs.end(); ++it, i++)
        it->setSolidColor(COLOR_WHITE, 0xFA);
}

// <gerstle> check for inputs
void LightWalker:: walk()
{
    int leg_count = _legs.size();
    // <gerstle> foreach leg, check the sensor
    TCL.sendEmptyFrame();
    for (vector<Leg>::iterator it = _legs.begin(); it != _legs.end(); ++it)
    {
        bool sensorState = digitalRead(it->triggerPin());
        if ((sensorState == LOW) && (it->lastTriggerStatus() == HIGH))
            it->flash();
        else if (sensorState == HIGH)
            it->setSolidColor(COLOR_BLACK, 0x00);
        it->setLastTriggerStatus(sensorState);
    }
}
