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

Leg::Leg(String n, int pin, int pixels, int brightness)
{
    Serial.print("initing..."); Serial.print(n); Serial.print(" - "); Serial.print(pin); Serial.print(" - "); Serial.println(pixels);

    name = n;
    trigger_pin = pin;
    pixel_count = pixels;
    max_brightness = brightness;
    status = Initialized;
    _mode = None;

    _fade_rate = FOOT_DOWN_FADE_STEP;

    fade_on = true;
    foot_down_fade_step = FOOT_DOWN_FADE_STEP;
    foot_up_fade_step = FOOT_UP_FADE_STEP;

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
    Serial.print("fade rate to -> "); Serial.println(foot_down_fade_step);
    _fade_rate = foot_down_fade_step;
    if (_fade_rate <= 0)
        _fade_rate = 2;
    flash();
}

void Leg::footup()
{
    Serial.print(name); Serial.println(" -> up");
    Serial.print("fade rate to -> "); Serial.println(foot_up_fade_step);
    _fade_rate = foot_up_fade_step;
    if (_fade_rate <= 0)
        _fade_rate = 2;
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
    Serial.print("    pixel_count:"); Serial.println(pixel_count);
    Serial.print("    lower foot border:"); Serial.println(lower_foot_border);
    Serial.print("    upper foot border:"); Serial.println(upper_foot_border);
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
    //Serial.println("sparkling " + name);
    byte level;
    int distance;
    for (int i = 0; i < lower_foot_border; i++)
    {
        distance = lower_foot_border - i;
        level = byte(random(0, ((max_brightness/(lower_foot_border * 3)) * (i + 1))));
        _pixels[i] = RGB(level, level, level);
    }

    for (int i = upper_foot_border + 1; i < pixel_count; i++)
    {
        // <gerstle> white sparkle
        level = byte(random(0, ((max_brightness/(lower_foot_border * 3)) * (lower_foot_border - (i - upper_foot_border) + 1))));
        _pixels[i].r = level;
        _pixels[i].g = level;
        _pixels[i].b = level;

        // <gerstle> random color sparkle
//         _pixels[i].r = byte(random(0, ((max_brightness/(lower_foot_border * 3)) * (lower_foot_border - (i - upper_foot_border) + 1))));
//         _pixels[i].g = byte(random(0, ((max_brightness/(lower_foot_border * 3)) * (lower_foot_border - (i - upper_foot_border) + 1))));
//         _pixels[i].b = byte(random(0, ((max_brightness/(lower_foot_border * 3)) * (lower_foot_border - (i - upper_foot_border) + 1))));
    }

    displayPixels();
    setMode(Sparkle);
}

void Leg::fade()
{
    if (fade_on)
    {
//         Serial.println("fading " + name);
        bool still_fading = false;

        for (int i = 0; i < pixel_count; i++)
            if (((_pixels[i].r - _fade_rate) > 0x00) ||
                ((_pixels[i].g - _fade_rate) > 0x00) ||
                ((_pixels[i].b - _fade_rate) > 0x00))
            {
                
                Serial.print("fade "); Serial.print(name); Serial.print(" to: "); Serial.print(_pixels[i].r - _fade_rate); Serial.print(" - "); Serial.print(_pixels[i].g - _fade_rate); Serial.print(" - "); Serial.println(_pixels[i].b - _fade_rate);
                setPixel(i, _pixels[i], _fade_rate);
                still_fading = true;
            }
            else
                setPixel(i, COLOR_BLACK, 0x00);

            if (still_fading)
                setMode(Fade);
            else
                off();
    }
    else
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

    if (false)
    {
        Serial.print("setPixel:: "); Serial.print(_pixels[i].r); Serial.print(" - "); Serial.print(_pixels[i].g); Serial.print(" - "); Serial.print(_pixels[i].b); Serial.print(" dim: "); Serial.println(dimmer);
    }

    // <gerstle> if any of the colors are over max_brightness,
    // step down all the pixels. hopefully that doesn't change
    // the color
    int diff;
    if (_pixels[i].r > max_brightness)
    {
        diff = _pixels[i].r - max_brightness;
        _pixels[i].r -= diff;
        _pixels[i].g -= diff;
        _pixels[i].b -= diff;
    }

    if (_pixels[i].g > max_brightness)
    {
        diff = _pixels[i].g - max_brightness;
        _pixels[i].r -= diff;
        _pixels[i].g -= diff;
        _pixels[i].b -= diff;
    }

    if (_pixels[i].b > max_brightness)
    {
        diff = _pixels[i].b - max_brightness;
        _pixels[i].r -= diff;
        _pixels[i].g -= diff;
        _pixels[i].b -= diff;
    }

    if (false)
    {
        Serial.print("setPixel:: "); Serial.print(_pixels[i].r); Serial.print(" - "); Serial.print(_pixels[i].g); Serial.print(" - "); Serial.print(_pixels[i].b); Serial.print(" dim: "); Serial.println(dimmer);
    }

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

LightWalker::LightWalker(int brightness)
{
    _max_brightness = brightness;
}

void LightWalker::initLegs()
{
    TCL.sendEmptyFrame();

//     _legs[0] = Leg("left leg", TCL_MOMENTARY1, 50);
    _legs[0] = Leg("left leg", TCL_MOMENTARY1, 25, _max_brightness);
    _legs[1] = Leg("right leg", TCL_MOMENTARY2, 25, _max_brightness);
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
        
        if (displayStatus && DEBUG)
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

        if (digitalRead(TCL_SWITCH2) == HIGH)
            _legs[i].fade_on = true;
        else
            _legs[i].fade_on = false;

//         _legs[i].foot_down_fade_step = analogRead(TCL_POT1);
//         _legs[i].foot_up_fade_step = analogRead(TCL_POT4);
        _legs[i].max_brightness = analogRead(TCL_POT3);

        if (displayStatus && DEBUG)
        {
            Serial.print("    brightness "); Serial.println(_legs[i].max_brightness);
        }
    }
    TCL.sendEmptyFrame();
}
